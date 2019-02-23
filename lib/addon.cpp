#include <nan.h>
#include <Commctrl.h>
#include <CommonControls.h>
#include <Windows.h>
#include <cstddef>
#include <cwchar>
#include <memory>
#include <algorithm>
#include <vector>

namespace Gdiplus
{
using std::max;
using std::min;
} // namespace Gdiplus
#include <Gdiplus.h>

class ComInit
{
public:
  ComInit()
  {
    CoInitializeEx(0, COINIT_MULTITHREADED);
  }

  ~ComInit()
  {
    CoUninitialize();
  }

private:
  ComInit(const ComInit &);
  ComInit &operator=(const ComInit &);
};

class GdiPlusInit
{
public:
  GdiPlusInit()
  {
    Gdiplus::GdiplusStartupInput startupInput;
    Gdiplus::GdiplusStartup(std::addressof(this->token),
                            std::addressof(startupInput), nullptr);
  }

  ~GdiPlusInit()
  {
    Gdiplus::GdiplusShutdown(this->token);
  }

private:
  GdiPlusInit(const GdiPlusInit &);
  GdiPlusInit &operator=(const GdiPlusInit &);

  ULONG_PTR token;
};

struct IStreamDeleter
{
  void operator()(IStream *pStream) const
  {
    pStream->Release();
  }
};

std::unique_ptr<Gdiplus::Bitmap> CreateBitmapFromIcon(
    HICON hIcon, std::vector<std::int32_t> &buffer)
{
  ICONINFO iconInfo = {0};
  GetIconInfo(hIcon, std::addressof(iconInfo));

  BITMAP bm = {0};
  GetObject(iconInfo.hbmColor, sizeof(bm), std::addressof(bm));

  std::unique_ptr<Gdiplus::Bitmap> bitmap;

  if (bm.bmBitsPixel == 32)
  {
    auto hDC = GetDC(nullptr);

    BITMAPINFO bmi = {0};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = bm.bmWidth;
    bmi.bmiHeader.biHeight = -bm.bmHeight;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    auto nBits = bm.bmWidth * bm.bmHeight;
    buffer.resize(nBits);
    GetDIBits(hDC, iconInfo.hbmColor, 0, bm.bmHeight, std::addressof(buffer[0]),
              std::addressof(bmi), DIB_RGB_COLORS);

    auto hasAlpha = false;
    for (std::int32_t i = 0; i < nBits; i++)
    {
      if ((buffer[i] & 0xFF000000) != 0)
      {
        hasAlpha = true;
        break;
      }
    }

    if (!hasAlpha)
    {
      std::vector<std::int32_t> maskBits(nBits);
      GetDIBits(hDC, iconInfo.hbmMask, 0, bm.bmHeight,
                std::addressof(maskBits[0]), std::addressof(bmi),
                DIB_RGB_COLORS);

      for (std::int32_t i = 0; i < nBits; i++)
      {
        if (maskBits[i] == 0)
        {
          buffer[i] |= 0xFF000000;
        }
      }
    }

    bitmap.reset(new Gdiplus::Bitmap(
        bm.bmWidth, bm.bmHeight, bm.bmWidth * sizeof(std::int32_t),
        PixelFormat32bppARGB,
        static_cast<BYTE *>(static_cast<void *>(std::addressof(buffer[0])))));

    ReleaseDC(nullptr, hDC);
  }
  else
  {
    bitmap.reset(Gdiplus::Bitmap::FromHICON(hIcon));
  }

  DeleteObject(iconInfo.hbmColor);
  DeleteObject(iconInfo.hbmMask);

  return bitmap;
}

int GetEncoderClsid(const WCHAR *format, CLSID *pClsid)
{
  UINT num = 0u;
  UINT size = 0u;

  Gdiplus::GetImageEncodersSize(std::addressof(num), std::addressof(size));

  if (size == 0u)
  {
    return -1;
  }

  std::unique_ptr<Gdiplus::ImageCodecInfo> pImageCodecInfo(
      static_cast<Gdiplus::ImageCodecInfo *>(static_cast<void *>(new BYTE[size])));

  if (pImageCodecInfo == nullptr)
  {
    return -1;
  }

  GetImageEncoders(num, size, pImageCodecInfo.get());

  for (UINT i = 0u; i < num; i++)
  {
    if (std::wcscmp(pImageCodecInfo.get()[i].MimeType, format) == 0)
    {
      *pClsid = pImageCodecInfo.get()[i].Clsid;
      return i;
    }
  }

  return -1;
}

std::vector<unsigned char> HIconToPNG(HICON hIcon)
{
  GdiPlusInit init;

  std::vector<std::int32_t> buffer;
  auto bitmap = CreateBitmapFromIcon(hIcon, buffer);

  CLSID encoder;
  if (GetEncoderClsid(L"image/png", std::addressof(encoder)) == -1)
  {
    return std::vector<unsigned char>{};
  }

  IStream *tmp;
  if (CreateStreamOnHGlobal(nullptr, TRUE, std::addressof(tmp)) != S_OK)
  {
    return std::vector<unsigned char>{};
  }

  std::unique_ptr<IStream, IStreamDeleter> pStream{tmp};

  if (bitmap->Save(pStream.get(), std::addressof(encoder), nullptr) !=
      Gdiplus::Status::Ok)
  {
    return std::vector<unsigned char>{};
  }

  STATSTG stg = {0};
  LARGE_INTEGER offset = {0};

  if (pStream->Stat(std::addressof(stg), STATFLAG_NONAME) != S_OK ||
      pStream->Seek(offset, STREAM_SEEK_SET, nullptr) != S_OK)
  {
    return std::vector<unsigned char>{};
  }

  std::vector<unsigned char> result(
      static_cast<std::size_t>(stg.cbSize.QuadPart));
  ULONG ul;

  if (pStream->Read(std::addressof(result[0]),
                    static_cast<ULONG>(stg.cbSize.QuadPart),
                    std::addressof(ul)) != S_OK ||
      stg.cbSize.QuadPart != ul)
  {
    return std::vector<unsigned char>{};
  }

  return result;
}

std::wstring Utf8ToWide(const std::string &src)
{
  const auto size =
      MultiByteToWideChar(CP_UTF8, 0u, src.data(), -1, nullptr, 0u);
  std::vector<wchar_t> dest(size, L'\0');

  if (MultiByteToWideChar(CP_UTF8, 0u, src.data(), -1, dest.data(),
                          dest.size()) == 0)
  {
    throw std::system_error{static_cast<int>(GetLastError()),
                            std::system_category()};
  }

  return std::wstring{dest.begin(), dest.end()};
}

std::vector<unsigned char> GetIcon(const std::string &name, int size, UINT flag)
{
  ComInit init;

  flag |= SHGFI_ICON;

  switch (size)
  {
  case 16:
    flag |= SHGFI_SMALLICON;
    break;
  case 32:
    flag |= SHGFI_LARGEICON;
    break;
  case 64:
  case 256:
    flag |= SHGFI_SYSICONINDEX;
    break;
  }

  SHFILEINFOW sfi = {0};
  auto hr = SHGetFileInfoW(Utf8ToWide(name).c_str(), 0, std::addressof(sfi), sizeof(sfi), flag);
  HICON hIcon;

  if (FAILED(hr))
  {
    return std::vector<unsigned char>{};
  }

  if (size == 16 || size == 32)
  {
    hIcon = sfi.hIcon;
  }
  else
  {
    HIMAGELIST *imageList;
    hr = SHGetImageList(
        size == 64 ? SHIL_EXTRALARGE : SHIL_JUMBO, IID_IImageList,
        static_cast<void **>(static_cast<void *>(std::addressof(imageList))));

    if (FAILED(hr))
    {
      return std::vector<unsigned char>{};
    }

    hr = static_cast<IImageList *>(static_cast<void *>(imageList))
             ->GetIcon(sfi.iIcon, ILD_TRANSPARENT, std::addressof(hIcon));

    if (FAILED(hr))
    {
      return std::vector<unsigned char>{};
    }
  }

  auto buffer = HIconToPNG(hIcon);
  DestroyIcon(hIcon);
  return buffer;
}

void getIcon(const Nan::FunctionCallbackInfo<v8::Value> &info)
{

  if (info.Length() < 1)
  {
    Nan::ThrowTypeError("Wrong number of arguments");
    return;
  }

  if (!info[0]->IsString())
  {
    Nan::ThrowTypeError("Wrong arguments");
    return;
  }

  v8::String::Utf8Value path{info[0]->ToString()};
  auto data = GetIcon(*path, info[1]->Int32Value(), 0);

  uint8_t *arr = &data[0];

  info.GetReturnValue().Set(Nan::CopyBuffer(static_cast<char *>(static_cast<void *>(arr)), data.size()).ToLocalChecked());
}

void InitAll(v8::Handle<v8::Object> exports)
{
  exports->Set(Nan::New("getIcon").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(getIcon)->GetFunction());
}

NODE_MODULE(addon, InitAll);
