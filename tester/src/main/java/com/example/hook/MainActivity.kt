package com.example.hook

import android.os.Bundle
import android.util.Log
import android.widget.Button
import androidx.appcompat.app.AppCompatActivity
import com.example.hooktester.R

class MainActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        HookAllMethod()
            .addListener()

        findViewById<Button>(R.id.button)
            .setOnClickListener {
                Log.e("TAG", "onCreate: ");
            }

    }
}