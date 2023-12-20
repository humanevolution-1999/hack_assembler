#include<iostream>
#include<stdio.h>
#include<fstream>
#include "symbol_table.h"
#include "coder.h"
#include "parser.h"
#include "filehandler.h"


int main()
{
    //store line buffer from input file
    std::string line;
    
    //parses symbols in first go
    FileHandler file_1_symbol_parse("sample.txt");

    while(std::getline(file_1_symbol_parse.inputFile,line))
    {
        //increment line_count
        file_1_symbol_parse.line_num++;
        
        //check for instruction type
        if(instruction_type(line)==a_instruction)
        {
            std::string parsed_add = parse_variable(line);

            //handling for invalid symbols
            if(symbol_type(parsed_add)==invalid)
            {
                std::cerr<<"line "<<file_1_symbol_parse.line_num<<": invalid instruction";
                exit(0);
            }
            else if(symbol_type(parsed_add)==variable)
            {
                //add an unknown variable
                if(is_internal_symbol(parsed_add)==false)
                {
                    if(is_external_symbol_defined(parsed_add)==false)
                        add_variable_symbol(parsed_add);
                } 
            }
        }
        else if (instruction_type(line)==l_instruction)
        {
            //parse the label 
            std::string parsed_var= parse_label(line);

            //handling for invalid label name
            if(symbol_type(parsed_var)==invalid)
            {
                std::cerr<<"line "<<file_1_symbol_parse.line_num<<": invalid instruction";
                exit(0);
            }
            else if(symbol_type(parsed_var)==variable)
            {
                //add the label variable to the label table
                if(is_label_symbol_defined(parsed_var)==false)
                    add_label_symbol(parsed_var,file_1_symbol_parse.line_num+1);
            }
        }  
    }

    //parses lines in second go
    FileHandler file_1_line_parse("sample.txt");

    //create an output file
    std::ofstream outputFile("output.txt");

    //check if the file is succesfully opened
    if(!outputFile.is_open())
    {
        std::cerr << "Error opening the output file." << std::endl;
        return 1;  // Return an error code
    }

    while(std::getline(file_1_line_parse.inputFile,line))
    {
        //increments line count
        file_1_line_parse.line_num++;

        if(instruction_type(line)==a_instruction)
        {
            std::string parsed_var = parse_variable(line);
            if(symbol_type(parsed_var)==constant)
            {
                //convert constant to 16 bit binary
                std::string temp = get_constant_binary(parsed_var);
                outputFile<<temp<<"\n";
            }
            else if(symbol_type(parsed_var)==variable)
            {
                //convert variable to 16 bit binary. Also the variable name is guaranteed to be syntactically correct since it's already checked in the first pass
                std::string temp = get_variable_symbol(parsed_var);
                outputFile<<temp<<"\n";
            }
        }
        else if(instruction_type(line)==l_instruction)
        {
            std::string parsed_label = parse_label(line);
            if(symbol_type(parsed_label)==constant)
            {
                std::string temp = get_constant_binary(parsed_label);
                outputFile<<temp<<"\n";
            }
            else if(symbol_type(parsed_label)==variable)
            {
                //convert label to 16 bit binary. Also the label name is guaranteed to be syntactically correct since it's already checked in the first pass
                std::string temp = get_label_symbol(parsed_label);
                outputFile<<temp<<"\n";
            }
        }
        else if(instruction_type(line)==c_instruction)
        {
            std::string out_instruction = "111";

            //get destination part of the c instruction
            std::string dest_binary;
            dest_binary = parse_c_instruction_dest(line);
            //std::cout<<"dest "<<dest_binary<<"\n";
            if(check_validity_dest(dest_binary)==false)
            {
                std::cerr<<"Line "<<file_1_line_parse.line_num<<": invalid destination instruction";
                exit(0);
            }
            else
                //convert to binary destn instn
                out_instruction += get_dest_binary(dest_binary);

            //get comp part of the c instruction
            std::string comp_binary;
            comp_binary = parse_c_instruction_comp(line);
           
            //error handling in case of invalid computation instruction
            if(check_validity_comp(comp_binary)==false)
            {
                std::cerr<<"Line "<<file_1_line_parse.line_num<<": invalid computation instruction"<<"\n";
                exit(0);
            }
            else
                //convert to binary comp instn
                out_instruction += get_comp_binary(comp_binary);

            //get jump part of the c instruction
            std::string jump_binary;
            jump_binary = parse_c_instruction_jmp(line);

            //error handling in case of invalid jump instruction
            if(check_validity_jump(jump_binary)==false)
            {
                std::cerr<<"Line "<<file_1_line_parse.line_num<<": invalid jump instruction";
                exit(0);
            }
            else
                //convert to binary jump instn
                out_instruction +=get_jump_binary(jump_binary);

            outputFile<<out_instruction<<"\n";
        }
    }

    outputFile.close();
    //file_1.m_inputFile("sample.txt");
    return 0;
}
