#include "gtest/gtest.h"

#include <rtl-sdr.h>
#include <string.h>

TEST(RtlSdr, GetDeviceCount)
{
    EXPECT_EQ(rtlsdr_get_device_count(), 1);
}

TEST(RtlSdr, GetDeviceName)
{
    std::string deviceName(rtlsdr_get_device_name(1));
    EXPECT_EQ(deviceName, "");
}

TEST(RtlSdr, GetUsbString)
{
  char mfg[256] = "";
  char prod[256] = "";
  char serial[256] = "";
  int res = rtlsdr_get_device_usb_strings(0, mfg, prod, serial);
  EXPECT_EQ(res, 0);
  std::string mfgstr(mfg);
  std::string prodstr(prod);
  std::string serialstr(serial);
  EXPECT_EQ(mfgstr, "Realtek");
  EXPECT_EQ(prodstr, "RTL2838UHIDIR");
  EXPECT_EQ(serialstr, "");
}

TEST(RtlSdr, OpenCloseDevice)
{
  rtlsdr_dev_t *dev;
  int res = rtlsdr_open(&dev, 0);
  EXPECT_EQ(res, 0);
  res = rtlsdr_close(dev);
  EXPECT_EQ(res, 0);
}
