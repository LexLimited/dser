#include <dser/desktop_file.h>

#include <dser/file.h>

int dser::desktop_file::read(std::string_view path)
{
    dser::fs::file file;
    int err = file.open(path.data());
    if (err) return err;

    

    return 0;
}
