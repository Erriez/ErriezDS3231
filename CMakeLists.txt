cmake_minimum_required(VERSION 3.5)

idf_component_register(SRCS "src/ErriezDS3231.cpp" 
                       INCLUDE_DIRS "." "src/"
                       REQUIRES arduino)

project(ErriezDS3231)
