package com.example.hook.ui

import android.app.Application
import com.example.hook.server.Ktor
import kotlin.concurrent.thread

class Application : Application() {

    override fun onCreate() {
        super.onCreate()
        Ktor.start()
    }

}