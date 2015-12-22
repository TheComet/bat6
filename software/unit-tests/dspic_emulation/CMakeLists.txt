include_directories ("include")
add_library (dspic_emulation SHARED
    "src/libq.cpp"
    "src/p33EP16GS506.cpp"
    "src/compiler_symbols.cpp")

