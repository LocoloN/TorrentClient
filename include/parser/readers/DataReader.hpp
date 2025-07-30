#pragma once
#include <filesystem>
#include <vector>
#include <optional>
    class SimpleDataReader;

    namespace TorrentClient { 
        enum readerType {
            simple = 1
        };

    template<typename data_type = unsigned char> 
    class DataReader {
    protected:
        std::filesystem::path used_file_path{""};
        inline virtual bool is_good() const = 0;
        DataReader() = default;

    private:
    public:
        virtual bool is_open() const noexcept = 0;
        virtual bool open_file(const std::filesystem::path &path) noexcept = 0;
        virtual bool close() noexcept;
        static std::unique_ptr<DataReader> create(readerType param);
        virtual std::optional<data_type> operator[](const std::size_t &index)  const noexcept = 0; 
        virtual std::optional<std::vector<unsigned char>> get_block(const std::size_t &offset, size_t size) const noexcept = 0;

     };
    template <typename data_type>
    std::unique_ptr<DataReader<data_type>> DataReader<data_type>::create(readerType param) {
        switch (param)
        {
        case readerType::simple : {
            return std::make_unique<SimpleDataReader>();
        }
        default: {
            return nullptr;
        }
        }
    }
}
