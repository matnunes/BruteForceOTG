# Python program for reading
# from file
 
 
inFile = open('Password_Default_ProbWL.txt', 'r')
outFile = open('output.txt', 'w')
    
# Reading from the file
content = inFile.readlines()
 
for line in content:
	if (line.rstrip().isnumeric()):
		outFile.write(line)
