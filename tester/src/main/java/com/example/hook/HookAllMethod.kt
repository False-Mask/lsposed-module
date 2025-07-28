package com.example.hook

class HookAllMethod {

    init {
        System.loadLibrary("tester")
    }

    external fun addListener();

}