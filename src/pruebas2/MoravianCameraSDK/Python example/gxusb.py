# -*- coding: utf-8 -*-

from __future__ import print_function
import sys, os
from ctypes import *

# enumproc
cam_id = None

def py_enum_proc(c_id):
    global cam_id
    cam_id = c_id
    print('py_enum_proc() id:', cam_id)


ENUMPROC = CFUNCTYPE(None, c_uint)
enum_proc = ENUMPROC(py_enum_proc)

# GetValue indexes from gxusb.h
gvChipTemperature = 0
gvSupplyVoltage   = 10

# GetIntegerParameter indexes from gxusb.h
gipChipW = 1
gipChipD = 2

# GetStringParameter indexes from gxusb.h
gspCameraDescription = 0

# This example loads gxUsb library from the location of this script.
DLL_PATH = os.path.join(os.path.abspath(os.path.dirname(sys.argv[0])), 'gXusb.dll')


# This example does not check return values!
class GxCam:
    gxdll = CDLL(DLL_PATH)
    cam_id = None
    cam_handle = None
    w = None
    h = None
    bin_x = 1
    bin_y = 1

    def __init__(self):
        self.init_cdll()

    def init_cdll(self):
        # Init ctypes param types and return value to work in 64bit Python.
        self.gxdll.Initialize.restype = c_void_p
        self.gxdll.Release.argtypes = [c_void_p]
        self.gxdll.GetValue.argtypes = [c_void_p, c_uint32, c_void_p]
        self.gxdll.GetIntegerParameter.argtypes = [c_void_p, c_uint32, c_void_p]
        self.gxdll.GetStringParameter.argtypes = [c_void_p, c_uint32, c_uint32,
            c_void_p]
        self.gxdll.SetReadMode.argtypes = [c_void_p, c_uint32]
        self.gxdll.GetImageExposure.argtypes = [c_void_p, c_double, c_uint32,
            c_uint32, c_uint32, c_uint32, c_uint32, c_void_p]
        # We have to set restype to check the return value, too.
        self.gxdll.GetImageExposure.restype = c_bool
        self.gxdll.SetBinning.argtypes = [c_void_p, c_uint, c_uint]
        self.gxdll.GetLastErrorString.argtypes = [c_void_p, c_uint32, c_void_p]

    def enum_cameras(self):
        # This example uses only one camera. In case of multiple cameras
        # connected, last one enumerated is used.
        global cam_id
        self.gxdll.Enumerate(enum_proc)
        self.cam_id = cam_id

    def connect(self):
        if self.cam_id is not None:
            self.cam_handle = self.gxdll.Initialize(self.cam_id)
            self.w = self.get_integer_parameter(gipChipW)
            self.h = self.get_integer_parameter(gipChipD)
            desc = self.get_string_parameter(gspCameraDescription)
            print('Connected to camera', desc.strip(), 'with ChipW:', self.w,
                ', ChipH:', self.h)
            chip_temp = self.get_value(gvChipTemperature)
            print('Camera chip temperature:', chip_temp)

    def disconnect(self):
        if self.initialized():
            self.gxdll.Release(self.cam_handle)

    def get_value(self, value):
        if self.initialized():
            v = c_float(-1.0)
            self.gxdll.GetValue(self.cam_handle, value, byref(v))
            return v.value
        return -1.0

    def get_integer_parameter(self, param):
        if self.initialized():
            v = c_uint32()
            self.gxdll.GetIntegerParameter(self.cam_handle, param, byref(v))
            return v.value
        return -1

    def get_string_parameter(self, param):
        if self.initialized():
            buf = create_string_buffer(128)
            self.gxdll.GetStringParameter(self.cam_handle, param,
                sizeof(buf) - 1, buf)
            return buf.value
        return ''

    def set_binning(self, bin_x, bin_y):
        if self.initialized():
            self.bin_x = bin_x
            self.bin_y = bin_y
            self.gxdll.SetBinning(self.cam_handle, bin_x, bin_y)

    def get_image_exposure(self, exp_time):
        if self.initialized():
            # GetImageExposure needs binned width and height
            w = int(self.w / self.bin_x)
            h = int(self.h / self.bin_y)
            p_image = create_string_buffer(w * h * 2)
            self.gxdll.SetReadMode(self.cam_handle, 0)
            res = self.gxdll.GetImageExposure(self.cam_handle, c_double(exp_time),
                1, 0, 0, w, h, sizeof(p_image), p_image)
            if not res:
                p_error = create_string_buffer(2048)
                self.gxdll.GetLastErrorString(self.cam_handle,
                    sizeof(p_error) - 1, p_error)
                print('get_image_exposure() error: ', p_error.value)

    def initialized(self):
        return self.cam_handle is not None


if __name__ == '__main__':
    g = GxCam()
    g.enum_cameras()
    g.connect()
    # uncomment to test binning
    # g.set_binning(2, 2)
    g.get_image_exposure(0.5)
    g.disconnect()
