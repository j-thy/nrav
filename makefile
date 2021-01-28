  # the compiler: gcc for C program, define as g++ for C++
  CC = g++

  # compiler flags:
  #  -g    adds debugging information to the executable file
  #  -Wall turns on most, but not all, compiler warnings
  CFLAGS  = -std=c++11 -Wall

  # the build target executable:
  TARGET = nrav

  all: run

  run: $(TARGET)
	./$(TARGET) Derrick-Henry-Wk-14-2018 D.Henry 14 2018
	./$(TARGET) Dalvin-Cook-Wk-1-2020 D.Cook 1 2020
	./$(TARGET) Ezekiel-Elliott-Wk-6-2020 E.Elliott 6 2020
	./$(TARGET) Rudi-Johnson-Wk-10-2003 R.Johnson 10 2003
	./$(TARGET) LaDainian-Tomlinson-Wk-10-2006 L.Tomlinson 10 2006

  $(TARGET): $(TARGET).cpp
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).cpp

  clean:
	$(RM) $(TARGET)