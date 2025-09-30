#pragma once

#include <filesystem>
#include <functional>
#include <memory>
#include <optional>

namespace TorrentClient {

template <typename T> class LazyProperty {
protected:
  std::optional<std::unique_ptr<T>> ptr{};
  std::streampos _position;
  std::function<std::optional<T>()> _loader;

private:
public:
  LazyProperty() = delete;

  LazyProperty(const LazyProperty &param) noexcept = delete;

  LazyProperty(LazyProperty &&param) noexcept = default;
  /// @brief main constructor for lazy property
  /// @param loader is function that loads the returned property
  /// @param position of property inside file, if not set, than it is assumed
  /// that loader contains position inside itself
  LazyProperty(std::function<std::optional<T>()> loader,
               std::streampos position = -1)
      : _loader(std::move(loader)), _position(position) {}

  std::optional<std::unique_ptr<T>> Get() {
    if (ptr.has_value()) {
      return *ptr.value();
    }
    auto loadedValueBuffer = _loader();

    ptr = std::make_shared<T>((!loadedValueBuffer.has_value())
                                  ? std::nullopt
                                  : loadedValueBuffer.value());
    return ptr;
  }

  void Dispose() {
    if (!ptr.has_value())
      return;
    ptr.value().reset();
    ptr = std::nullopt;
  }
};
} // namespace TorrentClient
