#pragma once
#include <filesystem>
#include <vector>
#include <optional>

    namespace TorrentClient { 
    enum readerType {
        simple = 1
    };

    class DataReader {
    protected:
        std::filesystem::path used_file_path{""};
        inline virtual bool is_good() const = 0;

    private:
    public:
        DataReader() = default;
        virtual ~DataReader() = default;
        virtual bool is_open() const noexcept = 0;
        virtual bool open_file(const std::filesystem::path &path) noexcept = 0;
        virtual void close_file() noexcept = 0;
        static std::unique_ptr<DataReader> create(readerType param);
        virtual std::optional<unsigned char> operator[](const std::size_t &index)  const noexcept = 0; 
        virtual std::optional<std::vector<unsigned char>> get_block(std::size_t offset, std::size_t size) const noexcept = 0;

     };
}
