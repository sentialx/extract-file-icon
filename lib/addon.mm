#import <AppKit/AppKit.h>
#include <napi.h>
#include <vector>

std::vector<unsigned char> imageToPng(NSImage* image) {
  auto imageRef = [image CGImageForProposedRect:nil context:nil hints:nil];
  auto imageRep = [[NSBitmapImageRep alloc] initWithCGImage:imageRef];
  [imageRep setSize:[image size]];
  auto imageData = [imageRep representationUsingType:NSPNGFileType properties:[[NSDictionary alloc] init]];
  auto p = static_cast<const unsigned char*>([imageData bytes]);
  return std::vector<unsigned char>{p, p + [imageData length]};
}

Napi::Buffer<char> getIcon(const Napi::CallbackInfo &info) {
    Napi::Env env{info.Env()};

    auto path{info[0].As<Napi::String>().Utf8Value()};
    auto size{info[1].As<Napi::Number>().Int32Value()};

    auto image = [[NSWorkspace sharedWorkspace] iconForFile:[NSString stringWithUTF8String:path.c_str()]];
    [image setSize:NSMakeSize(size, size)];

    auto data = imageToPng(image);
    uint8_t *arr = &data[0];

    return Napi::Buffer<char>::Copy(
        env, static_cast<char *>(static_cast<void *>(arr)), data.size());
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set(Napi::String::New(env, "getIcon"),
                Napi::Function::New(env, getIcon));

    return exports;
}

NODE_API_MODULE(addon, Init)