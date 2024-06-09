#!/bin/bash

# Injecting xcpivacy as CMake won't do it
cp PrivacyInfo.xcprivacy build_ios/Release-iphoneos/corto_dll.framework

