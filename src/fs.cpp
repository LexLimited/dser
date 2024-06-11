#include <cstring>
#include <dirent.h>

#include <dser/fs.h>
#include <dser/log.h>
#include <dser/utils.h>
#include <strings.h>

namespace dser::fs
{

    dser::result<std::vector<std::string>, dser::error> ls(const std::string_view& path)
    {
        dser::fs::directory dir(path);
        if (dir.closed())
            return decltype(ls(""))::error(dser::error("Failed to open directory"));

        std::vector<std::string> names;
        for (auto it : dir)
            names.push_back(it.name());

        return names;
    }

    // directory::item

    directory::item::item(::dirent* ent, DIR* dir_stream)
        : _ent(ent), _dir_stream(dir_stream)
    {}

    const char* directory::item::name() const noexcept
    {
        return this->_ent->d_name;
    }

    directory::item::file_type directory::item::type() const noexcept
    {
        return this->dirent_d_type_to_file_type(this->_ent->d_type);
    }
    
    bool directory::item::operator==(const item& other) const
    {
        return this->_ent == other._ent;
    }

    bool directory::item::operator!=(const item& other) const
    {
        return this->_ent != other._ent;
    }

    dser::result<struct ::stat, int> directory::item::stats()
    {
        struct ::stat st;
        ::bzero(&st, sizeof(st));
        
        int err = ::fstatat(
                    ::dirfd(this->_dir_stream),
                    this->name(),
                    &st,
                    0);
        
        if (err)
            return decltype(stats())::error(err);

        return st;
    }

    directory::item::file_type directory::item::dirent_d_type_to_file_type(int d_type) const noexcept
    {
        switch (d_type)
        {
            case DT_REG: return file_type::REGULAR_FILE;
            case DT_DIR: return file_type::DIRECTORY;
            case DT_FIFO: return file_type::FIFO;
            case DT_SOCK: return file_type::SOCKET;
            case DT_CHR: return file_type::CHAR_DEVICE;
            case DT_BLK: return file_type::BLOCK_DEVICE;
            case DT_LNK: return file_type::SYM_LINK;
            default: return file_type::UNKNOWN;
        }
    }

    // directory::Iterator

    directory::Iterator::Iterator(DIR* stream) : _stream(stream)
    {
        if (!stream)
            this->_item = item(nullptr, nullptr);
        else
            this->_item = item(::readdir(this->_stream), this->_stream);
    }

    directory::Iterator::Iterator(DIR* stream, ::dirent* _ent)
        : _stream(stream), _item(_ent, stream)
    {}

    directory::Iterator& directory::Iterator::operator++()
    {
        if (this->_stream)
            this->_item = item(::readdir(this->_stream), this->_stream);
        
        return *this;
    }

    bool directory::Iterator::operator==(const Iterator& other) const
    {
        return this->_item == other._item;
    }

    bool directory::Iterator::operator!=(const Iterator& other) const
    {
        return this->_item != other._item;
    }

    directory::Iterator::reference directory::Iterator::operator*()
    {
        return this->_item;
    }

    // directory

    directory::directory(const std::string_view& path)
    {
        this->open(path);
    }

    int directory::open(const std::string_view path)
    {
        this->close();
        if (!(this->_stream = ::opendir(path.data())))
            return errno;

        return 0;
    }

    int directory::close()
    {
        if (!this->_stream)
            return 0;

        return ::closedir(this->_stream);
    }

    bool directory::closed() const
    {
        return !this->_stream;
    }

    int directory::fd() const
    {
        return ::dirfd(this->_stream);
    }

    directory::Iterator directory::begin() const
    {
        return Iterator(this->_stream);
    }

    directory::Iterator directory::end() const
    {
        return Iterator(nullptr, nullptr);
    }

    /// Lists files and directories
    dser::result<std::vector<std::string>, dser::error> ls(const std::string_view& path);
}
