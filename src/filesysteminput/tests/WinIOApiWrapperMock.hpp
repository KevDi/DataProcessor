#pragma once
#include <gmock/gmock.h>
#include <WinIOApiWrapper.hpp>
#include <WinIOApiWrapperImpl.hpp>

class WinIOApiWrapperMock : public filesysteminput::internal::WinIOApiWrapper {
public:

  WinIOApiWrapperMock() {
    ON_CALL(*this, CreateIOCompletionPortWrapper).WillByDefault([this]() {
      return impl_.CreateIOCompletionPortWrapper();
    });

    ON_CALL(*this, AssociateIOCompletionPortWithFileHandle).WillByDefault([this](HANDLE h1, HANDLE h2) {
      return impl_.AssociateIOCompletionPortWithFileHandle(h1, h2);
    });

    ON_CALL(*this, GetQueuedCompletionStatusWrapper).WillByDefault([this](HANDLE handle, LPDWORD lpDword, PULONG_PTR pulong, LPOVERLAPPED* overlap) {
      return impl_.GetQueuedCompletionStatusWrapper(handle, lpDword, pulong, overlap);
    });

    ON_CALL(*this, ReadDirectoryChangesWrapper).WillByDefault([this](HANDLE handle, std::vector<std::byte>& data, LPDWORD bytes_returned, OVERLAPPED* overlap) {
      return impl_.ReadDirectoryChangesWrapper(handle, data, bytes_returned, overlap);
    });
  }

  filesysteminput::internal::HandlePtr CreateFileWrapper(const std::string& path) override {
    return impl_.CreateFileWrapper(path);
  }
  MOCK_METHOD(filesysteminput::internal::HandlePtr, CreateIOCompletionPortWrapper, (), (override));
  MOCK_METHOD(bool, AssociateIOCompletionPortWithFileHandle, (HANDLE,
  HANDLE), (override));
  MOCK_METHOD(bool, GetQueuedCompletionStatusWrapper, (HANDLE, LPDWORD, PULONG_PTR, LPOVERLAPPED*), (
  override));
  MOCK_METHOD(bool, ReadDirectoryChangesWrapper, (HANDLE, std::vector<std::byte>&, LPDWORD,
  OVERLAPPED*), (override));

private:
  filesysteminput::internal::WinIOApiWrapperImpl impl_;
};