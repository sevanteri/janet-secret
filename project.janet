(declare-project
  :name "secret"
  :description "Janet library for libsecret"
  :author "Pete Sevander"
  :license "MIT"
  :url "https://github.com/sevanteri/janet-secret"
  :repo "git+https://github.com/sevanteri/janet-secret.git")

(declare-native
  :name "_secret"
  :source @["secret.c"]
  # pkg-config --cflags janet glib-2.0 libsecret-1
  :cflags @["-I/usr/local/include/janet/"
            "-I/usr/include/libsecret-1"
            "-I/usr/include/glib-2.0"
            "-I/usr/lib/x86_64-linux-gnu/glib-2.0/include"
            "-pthread"
            "-I/usr/include/libmount"
            "-I/usr/include/blkid"]
  # pkg-config --libs janet glib-2.0 libsecret-1
 :lflags @["-L/usr/local/lib"
           "-ljanet"
           "-lsecret-1"
           "-lgio-2.0"
           "-lgobject-2.0"
           "-lglib-2.0"])

(declare-source
  :source ["secret.janet"])
