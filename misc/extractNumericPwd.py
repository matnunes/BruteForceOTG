# Python program for reading text files and extracting lines with numbers only
 
inFile = open('200-years.txt', 'r')
outFile = open('output.txt', 'w')
    
# Reading from the file
content = inFile.readlines()
 
for line in content:
	if (line.rstrip().isnumeric()):
		outFile.write(line)
