# src/

# include settings
#INCLUDE_DIRECTORIES(${${CMAKE_PROJECT_NAME}_SOURCE_DIR}/lib)
#INCLUDE_DIRECTORIES(${${CMAKE_PROJECT_NAME}_SOURCE_DIR}/lib/include)
INCLUDE_DIRECTORIES(${${CMAKE_PROJECT_NAME}_SOURCE_DIR}/include)

# lib settings
LINK_DIRECTORIES(${${CMAKE_PROJECT_NAME}_BINARY_DIR}/lib)


# all related files
AUX_SOURCE_DIRECTORY(. DIR_TARGET)
ADD_LIBRARY (relfiles ${DIR_TARGET})

################################################################################

ADD_EXECUTABLE(consumer consumer.cc)

#  executable dependencies
ADD_DEPENDENCIES(consumer relfiles)
ADD_DEPENDENCIES(consumer argcvnw_s)
ADD_DEPENDENCIES(consumer rabbitmq)

#  executable target links
TARGET_LINK_LIBRARIES(consumer relfiles)
TARGET_LINK_LIBRARIES(consumer argcvnw_s)
TARGET_LINK_LIBRARIES(consumer rabbitmq)

#  executable properties
SET_TARGET_PROPERTIES(consumer PROPERTIES OUTPUT_NAME "consumer")

################################################################################

ADD_EXECUTABLE(producer producer.cc)

#  executable dependencies
ADD_DEPENDENCIES(producer relfiles)
ADD_DEPENDENCIES(producer argcvnw_s)
ADD_DEPENDENCIES(producer rabbitmq)

#  executable target links
TARGET_LINK_LIBRARIES(producer relfiles)
TARGET_LINK_LIBRARIES(producer argcvnw_s)
TARGET_LINK_LIBRARIES(producer rabbitmq)


#  executable properties
SET_TARGET_PROPERTIES(producer PROPERTIES OUTPUT_NAME "producer")


################################################################################



