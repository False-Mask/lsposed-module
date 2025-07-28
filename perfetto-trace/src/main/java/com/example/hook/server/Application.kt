package com.example.hook.server

import io.ktor.server.application.Application
import io.ktor.server.application.install

fun Application.module() {
//    install(CallLogging)
    configureRouting()
}