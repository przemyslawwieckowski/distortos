/**
 * \file
 * \brief LittlefsFile class implementation
 *
 * \author Copyright (C) 2018 Kamil Szczygiel http://www.distortec.com http://www.freddiechopin.info
 *
 * \par License
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
 * distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "distortos/FileSystem/littlefs/LittlefsFile.hpp"

#include "littlefsErrorToErrorCode.hpp"

#include "distortos/FileSystem/littlefs/LittlefsFileSystem.hpp"

#include "distortos/assert.h"

#include <mutex>

#include <fcntl.h>

namespace distortos
{

/*---------------------------------------------------------------------------------------------------------------------+
| public functions
+---------------------------------------------------------------------------------------------------------------------*/

LittlefsFile::~LittlefsFile()
{
	close();
}

int LittlefsFile::close()
{
	const std::lock_guard<LittlefsFile> lockGuard {*this};

	if (opened_ == false)
		return EBADF;

	opened_ = {};
	const auto ret = lfs_file_close(&fileSystem_.fileSystem_, &file_);
	return littlefsErrorToErrorCode(ret);
}

std::pair<int, off_t> LittlefsFile::getPosition()
{
	const std::lock_guard<LittlefsFile> lockGuard {*this};

	if (opened_ == false)
		return {EBADF, {}};

	const auto ret = lfs_file_tell(&fileSystem_.fileSystem_, &file_);
	if (ret < 0)
		return {littlefsErrorToErrorCode(ret), {}};

	return {{}, ret};
}

std::pair<int, off_t> LittlefsFile::getSize()
{
	const std::lock_guard<LittlefsFile> lockGuard {*this};

	if (opened_ == false)
		return {EBADF, {}};

	const auto ret = lfs_file_size(&fileSystem_.fileSystem_, &file_);
	if (ret < 0)
		return {littlefsErrorToErrorCode(ret), {}};

	return {{}, ret};
}

std::pair<int, struct stat> LittlefsFile::getStatus()
{
	const auto ret = getSize();
	if (ret.first != 0)
		return {ret.first, {}};

	struct stat status {};
	status.st_mode = S_IFREG;
	status.st_size = ret.second;
	return {{}, status};
}

std::pair<int, bool> LittlefsFile::isATerminal()
{
	const std::lock_guard<LittlefsFile> lockGuard {*this};

	if (opened_ == false)
		return {EBADF, {}};

	return {{}, {}};
}

int LittlefsFile::lock()
{
	return fileSystem_.lock();
}

std::pair<int, size_t> LittlefsFile::read(void* const buffer, const size_t size)
{
	const std::lock_guard<LittlefsFile> lockGuard {*this};

	if (opened_ == false)
		return {EBADF, {}};

	if (buffer == nullptr && size != 0)
		return {EINVAL, {}};

	const auto ret = lfs_file_read(&fileSystem_.fileSystem_, &file_, buffer, size);
	if (ret < 0)
		return {littlefsErrorToErrorCode(ret), {}};

	return {{}, ret};
}

int LittlefsFile::rewind()
{
	const std::lock_guard<LittlefsFile> lockGuard {*this};

	if (opened_ == false)
		return EBADF;

	const auto ret = lfs_file_rewind(&fileSystem_.fileSystem_, &file_);
	return littlefsErrorToErrorCode(ret);
}

std::pair<int, off_t> LittlefsFile::seek(const Whence whence, const off_t offset)
{
	const std::lock_guard<LittlefsFile> lockGuard {*this};

	if (opened_ == false)
		return {EBADF, {}};

	const auto ret = lfs_file_seek(&fileSystem_.fileSystem_, &file_, offset,
			whence == Whence::beginning ? LFS_SEEK_SET : whence == Whence::current ? LFS_SEEK_CUR : LFS_SEEK_END);
	if (ret < 0)
		return {littlefsErrorToErrorCode(ret), {}};

	return {{}, ret};

}

int LittlefsFile::synchronize()
{
	const std::lock_guard<LittlefsFile> lockGuard {*this};

	if (opened_ == false)
		return EBADF;

	const auto ret = lfs_file_sync(&fileSystem_.fileSystem_, &file_);
	return littlefsErrorToErrorCode(ret);
}

int LittlefsFile::unlock()
{
	return fileSystem_.unlock();
}

std::pair<int, size_t> LittlefsFile::write(const void* const buffer, const size_t size)
{
	const std::lock_guard<LittlefsFile> lockGuard {*this};

	if (opened_ == false)
		return {EBADF, {}};

	if (buffer == nullptr && size != 0)
		return {EINVAL, {}};

	const auto ret = lfs_file_write(&fileSystem_.fileSystem_, &file_, buffer, size);
	if (ret < 0)
		return {littlefsErrorToErrorCode(ret), {}};

	return {{}, ret};
}

/*---------------------------------------------------------------------------------------------------------------------+
| private functions
+---------------------------------------------------------------------------------------------------------------------*/

int LittlefsFile::open(const char* const path, const int flags)
{
	assert(opened_ == false);

	if (path == nullptr)
		return EINVAL;

	constexpr int mask {O_RDONLY | O_WRONLY | O_RDWR};
	int convertedFlags;
	if ((flags & mask) == O_RDONLY)
		convertedFlags = LFS_O_RDONLY;
	else if ((flags & mask) == O_WRONLY)
		convertedFlags = LFS_O_WRONLY;
	else if ((flags & mask) == O_RDWR)
		convertedFlags = LFS_O_RDWR;
	else
		return EINVAL;

	if ((flags & O_CREAT) != 0)
		convertedFlags |= LFS_O_CREAT;
	if ((flags & O_EXCL) != 0)
		convertedFlags |= LFS_O_EXCL;
	if ((flags & O_TRUNC) != 0)
		convertedFlags |= LFS_O_TRUNC;
	if ((flags & O_APPEND) != 0)
		convertedFlags |= LFS_O_APPEND;

	const auto ret = lfs_file_open(&fileSystem_.fileSystem_, &file_, path, convertedFlags);
	if (ret == LFS_ERR_OK)
		opened_ = true;
	return littlefsErrorToErrorCode(ret);
}

}	// namespace distortos
