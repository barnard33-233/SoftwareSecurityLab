# @author Jing Ziyu
# convert binary data to C-syntax char array
import argparse

parse = argparse.ArgumentParser()
parse.add_argument("file", help="file to convert")
args = parse.parse_args()
fname = args.file

f = open(fname, "rb")

content = bytes(f.read())
content_len = len(content)
print("char injection[{} + 1] = {{".format(content_len))
for byte in content:
    print("\'\\x{:02x}\'".format(byte), end=",")
print("0\n};")

f.close()

