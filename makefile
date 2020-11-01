CPLEX_VERSION = 12.7
# detecta se o sistema é de 32 ou 64 bits
N_BITS = $(shell getconf LONG_BIT)

SYSTEM = x86-64_linux

LIBFORMAT = static_pic

#### diretorios com as libs do cplex
CPLEXDIR  = /opt/ibm/ILOG/CPLEX_Studio1210/cplex
CONCERTDIR = /opt/ibm/ILOG/CPLEX_Studio1210/concert

CPLEXLIBDIR   = $(CPLEXDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CONCERTLIBDIR = $(CONCERTDIR)/lib/$(SYSTEM)/$(LIBFORMAT)

#### define o compilador
CPPC = g++
#############################

#### diretorios com os source files e com os objs files
SRCDIR = src
OBJDIR = obj
INCLUDEDIR = include
#############################


#### opcoes de compilacao e includes
# -fPIC generate position independent code for shared libraries
# -w disable warning (-Wall to enable all warnings and -Wextra to enable extra warnings)
# -DIL_STD to enable some STL features used by cplex
CCOPT = -g -O0 -fPIC -w -Wl,--gc-sections -ffunction-sections -g -DIL_STD -std=c++17 
CONCERTINCDIR = $(CONCERTDIR)/include
CPLEXINCDIR   = $(CPLEXDIR)/include
CCFLAGS = $(CCOPT) -I$(CPLEXINCDIR) -I$(CONCERTINCDIR) -I$(INCLUDEDIR)
#############################

#### flags do linker
#CCLNFLAGS = -L$(CPLEXLIBDIR) -lilocplex -lcplex -L$(CONCERTLIBDIR) -lconcert -lm -lpthread -lGL -lGLU -lglut -lboost_program_options

CCLNFLAGS = -g  -L$(CPLEXLIBDIR) -lilocplex -lconcert -lcplex -L$(CONCERTLIBDIR) -lm -lpthread -ldl 
#############################


#### lista de todos os srcs e todos os objs
SRCS = $(wildcard $(SRCDIR)/*.cpp)
OBJS = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRCS))
#############################

#### regra principal, gera o executavel
pfm: $(OBJS) 
	@echo  "\033[31m \nLinking all objects files: \033[0m"
	$(CPPC) $(OBJS) $(OBJSUTILS) -o $@ $(CCLNFLAGS)
############################

#inclui os arquivos de dependencias
-include $(OBJS:.o=.d)

#regra para cada arquivo objeto: compila e gera o arquivo de dependencias do arquivo objeto
#cada arquivo objeto depende do .c e dos headers (informacao dos header esta no arquivo de dependencias gerado pelo compiler)
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@echo  "\033[31m \nCompiling $<: \033[0m"
	$(CPPC) -I$(include) $(CCFLAGS) -c $< -o $@
	@echo  "\033[32m \ncreating $< dependency file: \033[0m"
	$(CPPC) -I$(include) -std=c++17  -MM $< > $(basename $@).d
	@mv -f $(basename $@).d $(basename $@).d.tmp #proximas tres linhas colocam o diretorio no arquivo de dependencias (g++ nao coloca, surprisingly!)
	@sed -e 's|.*:|$(basename $@).o:|' < $(basename $@).d.tmp > $(basename $@).d
	@rm -f $(basename $@).d.tmp


#delete objetos e arquivos de dependencia
clean:
	@echo "\033[31mcleaning obj directory \033[0m"
	@rm -f $(OBJDIR)/*.o $(OBJDIR)/*.d
	@rm pfm

dirs:
	@mkdir $(OBJDIR)

rebuild: clean pfm
