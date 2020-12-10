
import sys
import os
import subprocess

num_args = len(sys.argv)

if(num_args != 4):
	print("Expecting the following arguments: output_file local, number of iterations, size of the file")
	sys.exit(1)

arguments =  sys.argv
file_location = arguments[1]
num_iter = int(arguments[2])

if(num_iter < 1):
	print("Error, expecting the number of iterations to be non-zero")
	sys.exit(1)

file_size = int(arguments[3])

if(file_size < 1):
        print("Error, expecting the file size to be non-zero")
        sys.exit(1)

time = []
bandwidth = []

for x in range(0, num_iter):
	exec_string = 'dd if=/dev/zero of=' + file_location + ' bs=' + str(file_size) + 'KB count=1 oflag=dsync'
	res = subprocess.Popen(exec_string, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
	out, err = res.communicate()
	lines = err.splitlines()
	if 'failed' in err:
		print(err)
		sys.exit(1)
	print(lines[2])
	split = lines[2].split(' ')
	time.append(split[5])
	bandwidth.append(split[7]+split[8])
	
print(time)
print(bandwidth)
