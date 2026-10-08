#include "libultraship/bridge/resourcebridge.h"
#include "ship/Context.h"
#include <string>
#include <algorithm>
#include <chrono>

#include "fast/resource/type/Texture.h"
#include "ship/utils/StrHash64.h"
#include "ship/window/Window.h"


namespace {
// Temporary diagnostics: logs resource loads that take long enough to stall a frame.
struct SlowLoad {
    const char* name;
    uint64_t crc;
    std::chrono::steady_clock::time_point t0 = std::chrono::steady_clock::now();
    SlowLoad(const char* n, uint64_t c) : name(n), crc(c) {}
    ~SlowLoad() {
        const double ms = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - t0).count();
        if (ms >= 10.0) {
            if (name != nullptr) {
                SPDLOG_INFO("[frame-stats] slow resource load took {:.1f}ms name={}", ms, name);
            } else {
                SPDLOG_INFO("[frame-stats] slow resource load took {:.1f}ms crc={:#x}", ms, crc);
            }
        }
    }
};
} // namespace

std::shared_ptr<Ship::IResource> ResourceLoad(const char* name) {
    SlowLoad slowTimer(name, 0);
    return Ship::Context::GetRawInstance()->GetResourceManager()->LoadResource(name);
}

std::shared_ptr<Ship::IResource> ResourceLoad(uint64_t crc) {
    SlowLoad slowTimer(nullptr, crc);
    return Ship::Context::GetRawInstance()->GetResourceManager()->LoadResource(crc);
}

extern "C" {

uint64_t ResourceGetCrcByName(const char* name) {
    return CRC64(name);
}

const char* ResourceGetNameByCrc(uint64_t crc) {
    return Ship::Context::GetRawInstance()->GetResourceManager()->GetArchiveManager()->HashToCString(crc);
}

size_t ResourceGetSizeByName(const char* name) {
    return Ship::Context::GetRawInstance()->GetResourceManager()->GetResourceSize(name);
}

size_t ResourceGetSizeByCrc(uint64_t crc) {
    return Ship::Context::GetRawInstance()->GetResourceManager()->GetResourceSize(crc);
}

uint8_t ResourceGetIsCustomByName(const char* name) {
    return Ship::Context::GetRawInstance()->GetResourceManager()->GetResourceIsCustom(name);
}

uint8_t ResourceGetIsCustomByCrc(uint64_t crc) {
    return Ship::Context::GetRawInstance()->GetResourceManager()->GetResourceIsCustom(crc);
}

void* ResourceGetDataByName(const char* name) {
    SlowLoad slowTimer(name, 0);
    return Ship::Context::GetRawInstance()->GetResourceManager()->GetResourceRawPointer(name);
}

void* ResourceGetDataByCrc(uint64_t crc) {
    SlowLoad slowTimer(nullptr, crc);
    return Ship::Context::GetRawInstance()->GetResourceManager()->GetResourceRawPointer(crc);
}

uint16_t ResourceGetTexWidthByName(const char* name) {
    const auto res = std::static_pointer_cast<Fast::Texture>(ResourceLoad(name));

    if (res != nullptr) {
        return res->Width;
    }

    SPDLOG_ERROR("Given texture path is a non-existent resource");
    return -1;
}

uint16_t ResourceGetTexWidthByCrc(uint64_t crc) {
    const auto res = std::static_pointer_cast<Fast::Texture>(ResourceLoad(crc));

    if (res != nullptr) {
        return res->Width;
    }

    SPDLOG_ERROR("Given texture path is a non-existent resource");
    return -1;
}

uint16_t ResourceGetTexHeightByName(const char* name) {
    const auto res = std::static_pointer_cast<Fast::Texture>(ResourceLoad(name));

    if (res != nullptr) {
        return res->Height;
    }

    SPDLOG_ERROR("Given texture path is a non-existent resource");
    return -1;
}

uint16_t ResourceGetTexHeightByCrc(uint64_t crc) {
    const auto res = std::static_pointer_cast<Fast::Texture>(ResourceLoad(crc));

    if (res != nullptr) {
        return res->Height;
    }

    SPDLOG_ERROR("Given texture path is a non-existent resource");
    return -1;
}

size_t ResourceGetTexSizeByName(const char* name) {
    const auto res = std::static_pointer_cast<Fast::Texture>(ResourceLoad(name));

    if (res != nullptr) {
        return res->ImageDataSize;
    }

    SPDLOG_ERROR("Given texture path is a non-existent resource");
    return -1;
}

size_t ResourceGetTexSizeByCrc(uint64_t crc) {
    const auto res = std::static_pointer_cast<Fast::Texture>(ResourceLoad(crc));

    if (res != nullptr) {
        return res->ImageDataSize;
    }

    SPDLOG_ERROR("Given texture path is a non-existent resource");
    return -1;
}

void ResourceGetGameVersions(uint32_t* versions, size_t versionsSize, size_t* versionsCount) {
    auto list = Ship::Context::GetRawInstance()->GetResourceManager()->GetArchiveManager()->GetGameVersions();
    memcpy(versions, list.data(), std::min(versionsSize, list.size() * sizeof(uint32_t)));
    *versionsCount = list.size();
}

void ResourceLoadDirectoryAsync(const char* name) {
    Ship::Context::GetRawInstance()->GetResourceManager()->LoadResourcesAsync(name);
}

uint32_t ResourceHasGameVersion(uint32_t hash) {
    auto list = Ship::Context::GetRawInstance()->GetResourceManager()->GetArchiveManager()->GetGameVersions();
    return std::find(list.begin(), list.end(), hash) != list.end();
}

void ResourceLoadDirectory(const char* name) {
    Ship::Context::GetRawInstance()->GetResourceManager()->LoadResources(name);
}

void ResourceDirtyDirectory(const char* name) {
    Ship::Context::GetRawInstance()->GetResourceManager()->DirtyResources(name);
}

void ResourceDirtyByName(const char* name) {
    auto resource = ResourceLoad(name);

    if (resource != nullptr) {
        resource->Dirty();
    }
}

void ResourceDirtyByCrc(uint64_t crc) {
    auto resource = ResourceLoad(crc);

    if (resource != nullptr) {
        resource->Dirty();
    }
}

void ResourceUnloadByName(const char* name) {
    Ship::Context::GetRawInstance()->GetResourceManager()->UnloadResource(name);
}

void ResourceUnloadByCrc(uint64_t crc) {
    ResourceUnloadByName(ResourceGetNameByCrc(crc));
}

void ResourceUnloadDirectory(const char* name) {
    Ship::Context::GetRawInstance()->GetResourceManager()->UnloadResources(name);
}

uint32_t IsResourceManagerLoaded() {
    return Ship::Context::GetRawInstance()->GetResourceManager()->IsLoaded();
}
}
