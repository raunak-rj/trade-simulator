cmake_minimum_required(VERSION 3.15)
project(TradeSimulator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# =======================
# Dependencies
# =======================
find_package(Qt5 REQUIRED COMPONENTS Widgets)
find_package(Boost REQUIRED COMPONENTS system thread)
find_package(OpenSSL REQUIRED)

# =======================
# WebSocket Client Library
# =======================
add_library(ws_core STATIC
    websocket/webSocketClient.cpp
)

target_include_directories(ws_core PUBLIC
    ${CMAKE_SOURCE_DIR}/include 
    ${CMAKE_SOURCE_DIR}/websocket
    ${CMAKE_SOURCE_DIR}/thirdparty/websocketpp
    ${CMAKE_SOURCE_DIR}/thirdparty
)

target_link_libraries(ws_core PRIVATE
    Boost::system
    Boost::thread
    OpenSSL::SSL
    OpenSSL::Crypto
    pthread
)

# =======================
# Core Simulation / ML Logic
# =======================
add_library(core STATIC
    utils/featureExtractor.cpp
    utils/labelOrderbook.cpp
    utils/snapshotHandler.cpp
    utils/makerTakerLabel.cpp
    utils/trainLogistic.cpp
    utils/almgrenEstimator.cpp
    utils/trainSlippageModel.cpp
    models/slippageRegressor.cpp
    models/logisticRegressor.cpp
    models/almgrenChrissModel.cpp
)

target_include_directories(core PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/utils
    ${CMAKE_SOURCE_DIR}/models
    ${CMAKE_SOURCE_DIR}/thirdparty
)

target_link_libraries(core PRIVATE
    Boost::system
    Boost::thread
)

# =======================
# Main Executable (UI + WS + Training + Models all-in-one)
# =======================
add_executable(trade_simulator
    ui/mainwindow.cpp
    include/mainwindow.h
    src/main.cpp
)

target_include_directories(trade_simulator PRIVATE
    ${CMAKE_SOURCE_DIR}/ui
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/models
)

target_link_libraries(trade_simulator PRIVATE
    Qt5::Widgets
    core
    ws_core
    OpenSSL::SSL
    OpenSSL::Crypto
    pthread
)

set_target_properties(trade_simulator PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON
)

# =======================
# Copy data/ folder after build
# =======================
add_custom_command(TARGET trade_simulator POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/data ${CMAKE_BINARY_DIR}/data
)
