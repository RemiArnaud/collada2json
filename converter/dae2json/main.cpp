// Copyright (c) 2012, Motorola Mobility, Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of the Motorola Mobility, Inc. nor the names of its
//    contributors may be used to endorse or promote products derived from this
//    software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
// THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

//find . -name '*.dae' -exec dae2json {} \;

#include <iostream>

#include "DAE2JSONWriter.h"
#include "assert.h"

#define STDOUT_OUTPUT 0

enum ArgsState
{
    PARSE_INPUT_FILE_ARG = 0,
    PARSE_OUTPUT_FILE_ARG,
    PARSE_OPTIONS_ARG,
    PARSE_STATES_END
};

void usage(char* prog)
{
	fprintf(stderr,"\nUSAGE: %s [COLLADA inputFile] [JSON outputFile] [options] \n", prog);
}

static COLLADABU::URI __ReplacePathExtensionWithJSON(const COLLADABU::URI& inputFileURI)
{
    std::string pathDir = inputFileURI.getPathDir();
    
    std::string fileBase = inputFileURI.getPathFileBase();
    
    return COLLADABU::URI(pathDir + fileBase + ".json");
}

static bool __SetupCOLLADA2JSONArgs(int argc, char * const argv[], DAE2JSON::COLLADA2JSONArgs *converterArgs)
{
    assert(converterArgs);
        
    if (argc < 2) {
		fprintf(stderr, "%s: missing arguments\n", argv[0]);
		usage(argv[0]);
		exit(1);
        return false;
    }
    converterArgs->invertTransparency = false;
    
    for (int argIndex = 1, state = PARSE_INPUT_FILE_ARG ; argIndex < argc && state < PARSE_STATES_END ; state++) {
        switch (state) {
            case PARSE_INPUT_FILE_ARG:
                converterArgs->inputFile = COLLADABU::URI(argv[argIndex++]); 
                break;
            case PARSE_OUTPUT_FILE_ARG:
                if (argIndex == argc) {
                    converterArgs->outputFile = __ReplacePathExtensionWithJSON(converterArgs->inputFile); 
                } else if (argv[argIndex][0] == '-') { //check if we have an option instead of a destination path
                    converterArgs->outputFile = __ReplacePathExtensionWithJSON(converterArgs->inputFile); 
                    argIndex++;
                } else {
                    converterArgs->outputFile = COLLADABU::URI(argv[argIndex++]); 
                }
                break;
            case PARSE_OPTIONS_ARG:
                if (strcmp(argv[argIndex++], "-i") == 0) {
                    converterArgs->invertTransparency = true;
                    printf("[option] invert transparency: on\n");
                }

                break;
        }
    }
    
    return true;
}


int main (int argc, char * const argv[]) {
    DAE2JSON::COLLADA2JSONArgs converterArgs;
    
    printf("Collada2JSON [pre-alpha] 0.1\n");
    if (__SetupCOLLADA2JSONArgs( argc, argv, &converterArgs)) {
#if !STDOUT_OUTPUT
        FILE* fd = fopen(converterArgs.outputFile.getURIString().c_str(), "w");
        if (fd) {
            FileStream s(fd);
#else
            FileStream s(stdout);
#endif
            PrettyWriter <FileStream> jsonWriter(s);
            printf("converting:%s ...\n",converterArgs.inputFile.getPathFile().c_str());
            DAE2JSON::DAE2JSONWriter* writer = new DAE2JSON::DAE2JSONWriter(converterArgs, &jsonWriter);        
            writer->write();
            printf("[completed conversion]\n");
#if !STDOUT_OUTPUT
            fclose(fd);
            delete writer;
        }   
#endif
    }
    return 0;
}
