#/usr/bin/env python
import sys
import os

file_path = sys.argv[1]
target_dir = sys.argv[2]

file_content = open(file_path).read()
for i in xrange(1, 1000):
    target_file_path = os.path.join(target_dir, "input_%03d.txt" % i)
    open(target_file_path, "w").write(file_content)
