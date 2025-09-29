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

  LazyProperty(std::function<std::optional<T>()> loader,
               std::streampos position)
      : _loader(std::move(loader)), _position(position) {}

  std::optional<T> Get() {
    if (ptr.has_value()) {
      return *ptr.value();
    }
    std::optional<std::unique_ptr<T>> buffer = _loader();
    if (!buffer.has_value())
      return std::nullopt;
    ptr = buffer;
  }

  void Dispose() {
    if (!ptr.has_value())
      return;
    ptr.value().reset();
    ptr = std::nullopt;
  }
};
} // namespace TorrentClient
