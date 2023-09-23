import string

filepath = './FIT_QUAD_labppo.ratdb'
table_file =  open(filepath, 'r')
table_text = table_file.read()

pos_index = table_text.find("labppo_scintillator")
end_pos = table_text.find("}", pos_index)
start_pos = table_text.rfind("{", 0, pos_index)

if pos_index == -1:
    start_pos = -1
    end_pos = -2

print(start_pos, end_pos, pos_index)

replacement_string = "\nno table anymore \n"

new_string = table_text[:start_pos] + replacement_string + table_text[end_pos+1:]

new_file = open(filepath, 'w')
new_file.write(new_string)
new_file.close()
