
# Device configuration for Samsung Galaxy S5 mini SM-G800F (kminilte)

## Spec Sheet

| Feature                 | Specification                     |
| :---------------------- | :-------------------------------- |
| CPU                     | Quad-core 1.4 GHz                 |
| Chipset                 | Exynos 3 Quad 3470                |
| GPU                     | Mali-400MP4                       |
| Memory                  | 1.5GB RAM                         |
| Shipped Android Version | (4.4.2)                           |
| Storage                 | 16 GB                             |
| MicroSD                 | Up to 64GB                        |
| Battery                 | 2100 mAh                          |
| Dimensions              | 131.1 x 64.8 x 9.1 mm             |
| Display                 | 720 x 1280 pixels                 |
| Camera                  | 8 MP, 3264 × 2448 pixels          |
| Release Date            | August 2014                       |


## Device Picture

![Samsung Galaxy S5 mini](http://images.samsung.com/is/image/samsung/de_SM-G800FZBADBT_000246076_Front_blue?$DT-Gallery$ "Samsung Galaxy S5 mini")

----------

## How To Build

Although this device is not officially supported, the build-steps are not that different between devices. Most of the information on this "How to build" page also apply to this device: 
https://wiki.lineageos.org/devices/herolte/build

Requirements:

- At least 8GB RAM (build will stop without a warning with 6GB)
- 65GB (sources and build results) + 35GB (CCACHE)

Perform the following steps from the above "How to build":

1. Install the SDK

2. Install the Build Packages

3. Create the directories

	```sh
	$ mkdir -p ~/bin
	$ mkdir -p ~/android/system
	```
	
	*Note:* you can replace the build root "~/android/system" with any directory you like. Let's assume it is "~/android/system" in the following steps.

4. Install the repo command

	```sh
	$ curl https://storage.googleapis.com/git-repo-downloads/repo > ~/bin/repo
	$ chmod a+x ~/bin/repo
	```

5. Put the ~/bin directory in your path of execution

6. Initialize the LineageOS source repository

	```sh
	$ cd ~/android/system/
	$ repo init -u https://github.com/LineageOS/android.git -b lineage-15.1
	```

7. Download the source code

	```sh
	$ repo sync
	```

8. Patch sources

	8.1. Clone the local manifests with the following commands:
	
	```sh
	$ cd ~/android/system/
	$ git clone https://github.com/cm-3470/android_.repo_local_manifests -b lineage-15.1 .repo/local_manifests
	```
	
	However if you already obtained local manifests from a different device, just copy at least the following files into .repo/local_manifests :
	
	- https://github.com/cm-3470/android_.repo_local_manifests/blob/lineage-15.1/kminilte.xml
	- https://github.com/cm-3470/android_.repo_local_manifests/blob/lineage-15.1/common.xml
	
	8.2. Fetch device specific repos by synching all repos
		
	```sh
	$ repo sync
	```
	
	8.3. Apply device specific patches (also repeat this step whenever the patches in the directory "patch" are modified):
		
	```sh
	$ cd device/samsung/smdk3470-common/patch
	$ ./apply.sh
	```

9. Prepare the device-specific code

	This step is device specific and hence different from the "How to build".
	  
	```sh
	$ source build/envsetup.sh
	$ lunch lineage_kminilte-userdebug
	```
	
	These commands only have a temporary effect, you will have to perform these commands again,   when you use a new terminal window.

10. (optional) Turn on caching to speed up build

	Only if you want to rebuilt LineageOS multiple times you also should enable CCACHE.
	E.g. by adding the following lines to your $HOME/.bashrc:
	```
	export USE_CCACHE=1
	export ANDROID_CCACHE_DIR="/mnt/ccache" # Replace with your ccache dir
	export ANDROID_CCACHE_SIZE="50G" # Replace with your ccache size
	```

11. (optional) Setup the Java VM heap size for the Jack server:
	Java uses a default max. heap size of 1/4 of the installed RAM. Jack needs a minimum of 4 GB to work properly with Android N, so if your build environment has < 16 GB RAM you should add the following line to your $HOME/.bashrc:
	```
	export ANDROID_JACK_VM_ARGS="-Dfile.encoding=UTF-8 -XX:+TieredCompilation -Xmx4096m"
	```

12. Start the build
	
	```sh
	$ croot
	$ mka bacon
	```

### Build-results:

The build-process takes a lot of time - the initial build can take about 10h. If CCACHE is enabled, the next builds only take 3-4 hours.
Note that especially the linking process (ld) needs a huge amount of memory. The building process stops without a warning if only 6GB are present. In this case you can try to build with only one process (mka bacon -j1).

When the build-process finished the following files will be available:

	LineageOS image: out/target/product/kminilte/lineage-15.1-<date>-UNOFFICIAL-<device>.zip
	recovery: out/target/product/kminilte/recovery.img

### Rebuild:

If you want to rebuild the ROM, perform the following steps:

```sh
$ repo sync
$ mka clean
$ mka bacon
```

"repo sync" might fail due to local source code modifications. In this case temporarily revert the changes and reapply them after the update:

```sh
$ repo sync (-> fails due to local modifications, failing subprojects are listed in log-output)
```
  
For each failing subproject, revert the changes (e.g. with git reset --hard ...)

```sh
$ croot
$ repo sync
```

Now reapply the patches (see section about device/samsung/smdk3470-common/patch/apply.sh)

----------

## Copyright

```
#
# Copyright (C) 2017 The LineageOS Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
```
