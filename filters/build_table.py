#!/usr/bin/env python
#Python script to generate filter table for use by filter chain.
#Written by: David Wotherspoon
import os, sys

def print_blue(msgp, msg):
	print "\x1b[96m"+msgp+"\x1b[0m " + msg

def print_orange(msgp, msg):
	print "\x1b[33m"+msgp+"\x1b[0m " + msg

cdir = os.path.dirname(os.path.realpath(__file__))
out_file = open(cdir + "/filter_table.h", 'w')

filter_names = list() #lists to hold names, parameters descriptions and constructors
filter_descs = list()
filter_params = list()
filter_constructors = list()
filter_applys = list()

print_blue("Note:", "Populating filter_table.h...")
out_file.write("#ifndef FT_H\n#define FT_H\n")
for x in os.listdir(cdir):
	if x[-9:] == "_filter.h":
		print "Found filter: " + x
		out_file.write('#include "' + x + '"\n') #Write the include line.
		cur = open(cdir + '/' + x, 'r') #open file for reading
		n_found = False
		p_found = False
		d_found = False
		for line in cur: #read each line start looking for @name @params @desc
			if line[0:8] == "//@name " and not(n_found):
				filter_names.append(line[8:-1])
				n_found = True
			elif line[0:8] == "//@desc " and not(d_found):
				filter_descs.append(line[8:-1])
				d_found = True
			elif line[0:10] == "//@params " and not(p_found):
				filter_params.append(line[10:-1])
				p_found = True
			elif n_found and p_found and d_found: #found everything, stop.
				break
		if not(n_found and d_found and p_found):
			print_orange("Warning:", "Failed to find name, parameters and description for this filter.");
			if not(n_found):
				filter_names.append(x[0:-2])
			if not(d_found):
				filter_descs.append(x[0:-2])
			if not(p_found):
				filter_params.append("")
		filter_constructors.append("&" + x[0:-2] + "_create")
		filter_applys.append("&" + x[0:-2] + "_apply")
		cur.close()
#All the filters had valid data, finish
print_blue("Note:", "Make sure all filters listed above are in Makefile.")
out_file.write("#endif\n\n#define FTABLE_LEN " + str(len(filter_names)) + '''\n\n
extern char * ftable_names[];\n
extern char * ftable_descs[];\n
extern char * ftable_params[];\n
extern applyFilter ftable_applys[];\n
extern createFilter ftable_creators[];\n''')
out_file.close()

print_blue("Note:", "Populating filter_table.c...")
out_file = open(cdir + "/filter_table.c", 'w')

out_file.write('#include "filter_table.h"\n/* Automatically generated table. */\n\nchar * ftable_names[] = {\n')
for i in filter_names:
	out_file.write('\t\t\t"'+i+'", \n')

out_file.write("};\n\nchar * ftable_descs[] = {\n")
for i in filter_descs:
	out_file.write('\t\t\t"'+i+'", \n')

out_file.write("}; \n\nchar * ftable_params[] = {\n")
for i in filter_params:
	out_file.write('\t\t\t"'+i+'", \n')

out_file.write("}; \n\napplyFilter ftable_applys[] = {\n")
for i in filter_applys:
	out_file.write('\t\t\t'+i+', \n')

out_file.write("};\n\ncreateFilter ftable_creators[] = {\n")
for i in filter_constructors:
	out_file.write("\t\t\t" + i+', \n')
out_file.write("};")
out_file.close()
print "Done."
