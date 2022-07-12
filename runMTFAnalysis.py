#!/usr/bin/python3

import subprocess
from shlex import split

def main():
  print("Working")
  subprocess.call(split("aliroot -l"))

if __name__ == "__main__":
  main()
