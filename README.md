# rickascii
A crazy command line “Never Gonna Give You Up” player. (Or anything else, I guess.) Specifically, this is aimed at running on Tiny Core Linux. Inspired by [roll.sh](https://github.com/keroserene/rickrollrc/blob/master/roll.sh). [Apache License 2.0](https://www.apache.org/licenses/LICENSE-2.0.html).

## Putting it together
1. Have a copy of a video, any video.
2. Clone and compile [hiptext](https://github.com/jart/hiptext).
3. Use it to generate the frames. Tiny Core appeared to not have 256 color support (either that or it does and I was too lazy to work out why it wasn’t working) so I used this command line:
   ```bash
   ./hiptext --width 80 --height 25 --nocolor rick.mp4 > rickascii.frames
   ```
4. I bzip2 compressed the frames since the file is massive and I wanted to keep the image as small as possible:
   ```bash
   bzip2 -9 < rickascii.frames > rickascii.bz2
   ```
5. Using Audacity, I imported rick.mp4, converted the audio to mono (Tracks &rarr; Stereo Track to Mono), set the project rate to 11025 Hz, and exported as unsigned 8-bit PCM to rick.wav. Click File &rarr; Export, set the format to Other, then set the format to an unsigned 8-bit PCM WAV:
   ![Audacity export](https://i.imgur.com/iZO0C8n.png)  
   This was the best and smallest I could get on Tiny Core. On other systems, you should be able to use the even smaller GSM encoding - for me, 8-bit PCM was 2.3 MB, while GSM was 461 KB.
6. That was bzipped too:
   ```bash
   bzip2 -9 < rick.wav > rickascii.wav.bz2
   ```
7. Fairly obviously you need to compile rickascii.c. When testing on my Mac, I used this fairly simple clang command line:
   ```bash
   clang rickascii.c -DDEBUG -o rickascii
   ```
Running `make` will also do the same (warning: and run the program, so turn down your sound!)

When `DEBUG` is set, <kbd>^C</kbd> is allowed, and OS X’s `afplay` is used instead of `aplay`.

### On Tiny Core
To compile a binary usable on Tiny Core, I transferred the code to a Ubuntu machine and used gcc with some specific flags. First, I had to install gcc-multilib:

```bash
sudo apt-get install gcc-multilib
```

Then:

```bash
# Mac:
scp rickascii.c zoidberg:/tmp/rickascii.c

# Ubuntu:
cd /tmp
gcc rickascii.c -std=gnu99 -m32 -pipe -o rickascii

# Mac:
scp zoidberg:/tmp/rickascii .
```

`make tinycore` will do the same - set your Ubuntu machine’s user/hostname in the makefile. 

## Running
To run rickascii, you must place it in the same directory as rickascii.wav.bz2, and pipe it the contents of rickascii.bz2 via `bzcat`:

```bash
bzcat rickascii.bz2 | ./rickascii
```

## Making a Tiny Core image
Before you start this, you’ll want to grab a copy of [Core.iso](http://tinycorelinux.net/downloads.html) and read up on [remastering](http://wiki.tinycorelinux.net/wiki:remastering).

You can basically throw the three files anywhere you want in the filesystem (I chose `/rickascii`), and launch it from `/etc/init.d/tc-config`. Append this to the bottom of that file:

```bash
/bin/stty intr undef
/usr/bin/bzcat /rickascii/rickascii.bz2 | /rickascii/rickascii
/sbin/reboot
```

If you build this into an ISO and boot it, you’ll notice that there’s no sound. We’ll need to download `alsa` and friends:

* [alsa.tcz](http://tinycorelinux.net/5.x/x86/tcz/alsa.tcz)
* [alsa.tcz.dep](http://tinycorelinux.net/5.x/x86/tcz/alsa.tcz.dep)
* [alsa-config.tcz](http://tinycorelinux.net/5.x/x86/tcz/alsa-config.tcz)
* [alsa-modules.tcz](http://tinycorelinux.net/5.x/x86/tcz/alsa-modules-3.8.13-tinycore.tcz)
* [libasound.tcz](http://tinycorelinux.net/5.x/x86/tcz/libasound.tcz)
* [libpci.tcz](http://tinycorelinux.net/5.x/x86/tcz/libpci.tcz)
* [pciutils.tcz](http://tinycorelinux.net/5.x/x86/tcz/pciutils.tcz)
* [pciutils.tcz.dep](http://tinycorelinux.net/5.x/x86/tcz/pciutils.tcz.dep)

Place these in `/tmp/tce/optional`. I believe that `tc-config` will load these modules for you; if not, add these lines before the `bzcat` line:

```bash
/usr/bin/sudo -u tc /usr/bin/tce-load -i /tmp/tce/optional/alsa.tcz > /dev/null
/usr/local/tce.installed/*
```

Boom - that’s it.

### Extra stuff
Since Tiny Core is fairly general-purpose, I spent a little too much time optimizing the system to remove extra stuff that wasn’t needed. Particularly, `tc-config` was reduced from a hundred or so lines to only 8. 

I didn’t sort out how to delete unneeded drivers, which were taking a fairly large amount of space in the image. The `.tcz` packages could have also been unpacked into the system to prevent from having to do that at boot, but I encountered some problems I can’t really remember any more. On top of that, busybox could have been recompiled to remove the unneeded commands, or even just removed, with the C code handling `tc-config`’s job.
