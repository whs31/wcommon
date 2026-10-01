#include <radar/mms/common/lockfile.h>
#include <radar/mms/common/uuid.h>

#include <cerrno>
#include <fstream>
#include <iterator>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>

#ifdef _WIN32
#  include <windows.h>
#else
#  include <fcntl.h>
#  include <unistd.h>
#endif

namespace
{
  [[nodiscard]] std::string path_string(std::filesystem::path const& path)
  {
    return path.generic_string();
  }

  [[nodiscard]] std::string error_message(int const error_code)
  {
#ifdef _WIN32
    return std::error_code(error_code, std::system_category()).message();
#else
    return std::error_code(error_code, std::generic_category()).message();
#endif
  }

  void create_parent_directories(std::filesystem::path const& path)
  {
    auto const parent = path.parent_path();
    if(parent.empty())
      return;

    auto ec = std::error_code();
    std::filesystem::create_directories(parent, ec);
    if(ec)
      throw radar::mms::common::LockFileError::cannot_create_directory(parent, ec);
  }

#ifdef _WIN32
  void create_lock_file(std::filesystem::path const& path, std::string_view const contents)
  {
    auto const handle = ::CreateFileW(
      path.c_str(),
      GENERIC_WRITE,
      0,
      nullptr,
      CREATE_NEW,
      FILE_ATTRIBUTE_NORMAL,
      nullptr
    );

    if(handle == INVALID_HANDLE_VALUE) {
      auto const error = ::GetLastError();
      if(error == ERROR_FILE_EXISTS or error == ERROR_ALREADY_EXISTS)
        throw radar::mms::common::LockFileError::already_locked(path);
      throw radar::mms::common::LockFileError::cannot_create(path, static_cast<int>(error));
    }

    auto written = DWORD();
    if(
      ::WriteFile(handle, contents.data(), static_cast<DWORD>(contents.size()), &written, nullptr)
        == 0
      or written != static_cast<DWORD>(contents.size())
    ) {
      auto error = ::GetLastError();
      if(error == ERROR_SUCCESS)
        error = ERROR_WRITE_FAULT;
      ::CloseHandle(handle);
      std::filesystem::remove(path);
      throw radar::mms::common::LockFileError::cannot_write(path, static_cast<int>(error));
    }
    ::CloseHandle(handle);
  }
#else
  void create_lock_file(std::filesystem::path const& path, std::string_view const contents)
  {
    auto const fd = ::open(  // NOLINT(cppcoreguidelines-pro-type-vararg)
      path.c_str(),
      O_CREAT | O_EXCL | O_WRONLY,
      0666
    );
    if(fd == -1) {
      if(errno == EEXIST)
        throw radar::mms::common::LockFileError::already_locked(path);
      throw radar::mms::common::LockFileError::cannot_create(path, errno);
    }

    auto const written = ::write(fd, contents.data(), contents.size());
    if(std::cmp_not_equal(written, contents.size())) {
      auto const error = written == -1 ? errno : EIO;
      ::close(fd);
      std::filesystem::remove(path);
      throw radar::mms::common::LockFileError::cannot_write(path, error);
    }

    ::close(fd);
  }
#endif
}  // namespace

RADAR_COMMON_NAMESPACE_BEGIN

LockFileError LockFileError::already_locked(std::filesystem::path const& path)
{
  return LockFileError("lockfile is already locked: '{}'", path_string(path));
}

LockFileError LockFileError::cannot_create(std::filesystem::path const& path, int const error_code)
{
  return LockFileError(
    "failed to create lock file '{}': {}",
    path_string(path),
    error_message(error_code)
  );
}

LockFileError LockFileError::cannot_write(std::filesystem::path const& path, int const error_code)
{
  return LockFileError(
    "failed to write lock file '{}': {}",
    path_string(path),
    error_message(error_code)
  );
}

LockFileError LockFileError::cannot_create_directory(
  std::filesystem::path const& path,
  std::error_code const error_code
)
{
  return LockFileError(
    "failed to create lock file directory at '{}': {}",
    path_string(path),
    error_code.message()
  );
}

LockFile::LockFile(std::filesystem::path const& path)
  : m_locked(false)
  , m_path(path)
{}

LockFile::LockFile(std::filesystem::path&& path)
  : m_locked(false)
  , m_path(std::move(path))
{}

LockFile::LockFile(LockFile&& other) noexcept
  : m_locked(other.m_locked)
  , m_path(std::move(other.m_path))
  , m_token(std::move(other.m_token))
{
  other.m_locked = false;
}

LockFile& LockFile::operator=(LockFile&& other) noexcept
{
  if(this == &other)
    return *this;

  this->unlock();
  this->m_locked = other.m_locked;
  this->m_path = std::move(other.m_path);
  this->m_token = std::move(other.m_token);
  other.m_locked = false;
  return *this;
}

LockFile::~LockFile() { this->unlock(); }

std::filesystem::path const& LockFile::path() const noexcept { return this->m_path; }

bool LockFile::is_locked() const noexcept { return this->m_locked; }

bool LockFile::try_lock() noexcept
{
  if(this->m_locked)
    return false;
  try {
    this->lock();
  } catch(...) {
    return false;
  }
  return true;
}

void LockFile::lock()
{
  if(this->m_locked)
    throw LockFileError::already_locked(this->m_path);
  create_parent_directories(this->m_path);
  auto const token = std::string {"locked:"} + Uuid::new_v4().to_string();
  create_lock_file(this->m_path, token);
  this->m_token = token;
  this->m_locked = true;
}

void LockFile::unlock() noexcept
{
  if(not this->m_locked)
    return;

  auto input = std::ifstream(this->m_path, std::ios::binary);
  if(not input) {
    if(not std::filesystem::exists(this->m_path)) {
      this->m_locked = false;
      this->m_token.clear();
    }
    return;
  }

  auto const contents = std::
    string {std::istreambuf_iterator<char> {input}, std::istreambuf_iterator<char> {}};
  if(contents != this->m_token) {
    this->m_locked = false;
    this->m_token.clear();
    return;
  }

  auto ec = std::error_code();
  std::filesystem::remove(this->m_path, ec);
  if(not ec) {
    this->m_locked = false;
    this->m_token.clear();
  }
}

RADAR_COMMON_NAMESPACE_END
