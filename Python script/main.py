import os
import sys


def parse_line(_line):
    
    _output = ''
    
    x=_line.split('\t\t')
    
    data = x[2][:-1]
    
   # _output= "sigma_write(" + x[1] + "," + last_item +")"+'\r'
   
    _output= '{' + data +'},\n'
  
    return _output 
    
def parse_address(_line):
    
    _output = ''
    
    x=_line.split('\t\t')
    
    _output= x[1]
  
    return _output     
    
def get_size(fileobject):
    fileobject.seek(0,2) # move the cursor to the end of the file
    size = fileobject.tell()
    return size
    
def convert_file(_input_name, _output_name):

    with open(_input_name, 'r') as f_input:
        with open(_output_name, 'a') as f_output:
            cur_line = f_input.readline()
            line_count = 0
            parse_result = ''
            start_addr = parse_address(cur_line)
            while cur_line:
                line_count += 1
                parse_result += parse_line(cur_line)
                #f_output.write(parse_result)
                cur_line = f_input.readline()
            parse_result = parse_result[:-2]
            final_result = '\n#define '+ input_name.upper() + '_ADDR ' + start_addr + '\n' 
            #final_result += '#define '+ input_name + '_reg_count ' + str(line_count) + '\n' 
            final_result += '\nADI_REG_TYPE ' + input_name.upper() + '_DATA[' + str(line_count) + '][4] = {\n' + parse_result + '};' + '\n'
            f_output.write(final_result)
            #fsize = get_size(f_output)
            #f_output.truncate(fsize-1)
            
            
    return None


def add_suffix(_input_file):
    path, filename_w_ext = os.path.split(_input_file)
    filename, file_extension = os.path.splitext(filename_w_ext)
    _output_file = path + filename + '_converted' + '.h'
    return _output_file
    
def extract_name(_input_file):
    _path, _filename_w_ext = os.path.split(_input_file)
    _filename, _file_extension = os.path.splitext(_filename_w_ext)
    return _filename

if __name__=='__main__':
    input_file = sys.argv[1]
    input_name = extract_name(input_file)
    #output_name = add_suffix(input_name)
    #output_name = 'crossover.h'
    try:
        sys.argv[2]
    except IndexError:
        output_file = 'options_param.h'
    else:
        output_file = sys.argv[2]
    
    convert_file (input_file, output_file)
    
