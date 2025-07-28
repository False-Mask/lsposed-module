package com.example.hook.server

import io.ktor.server.application.Application
import io.ktor.server.engine.embeddedServer
import io.ktor.server.netty.Netty
import kotlin.concurrent.thread

object Ktor {

    fun start() {
        thread {
            embeddedServer(Netty, port = port, host = host, module = Application::module).start(wait = true)
        }
    }
}