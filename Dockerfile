# Use an official GCC runtime as a parent image
FROM gcc:latest

# Set the working directory in the container
WORKDIR /usr/src/myapp

# Copy the source and header files into the container
COPY src/ ./src/
COPY include/ ./include/

# Include other files needed for the program to run
COPY access_cards.csv ./

# Compile your program, ensuring gcc knows where to find the header files
# Here, we are assuming all .c files in the src directory need to be compiled.
RUN gcc -o access_control_system ./src/*.c -I./include/

# Run the program when the container launches
CMD ["./access_control_system"]