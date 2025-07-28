package com.example.test

import android.app.Activity
import android.os.Bundle
import android.view.View
import android.view.ViewGroup
import com.example.test.databinding.ActivityMainBinding
import com.wy.lib.wytrace.ArtMethodTrace

class MainActivity : Activity() {

    private val binding by lazy {
        ActivityMainBinding.inflate(layoutInflater)
    }

    private val textView by lazy {
        DelayView(this).also {
            it.layoutParams = ViewGroup.MarginLayoutParams(ViewGroup.LayoutParams.WRAP_CONTENT,ViewGroup.LayoutParams.WRAP_CONTENT)
        }
    }

    private val parent: ViewGroup by lazy {
        textView.parent as ViewGroup
    }

    private var viewState = View.VISIBLE


    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(binding.root)

        binding.btn.setOnClickListener {
//            if(viewState == View.VISIBLE) {
//                viewState = View.GONE
//                textView.visibility = View.GONE
//                //parent.removeView(textView)
//            } else {
//                viewState = View.VISIBLE
//                // parent.addView(textView)
//                textView.visibility = View.VISIBLE
//            }
            ArtMethodTrace.foreInterpretor()
            ArtMethodTrace.methodHookStart("",-1,Int.MAX_VALUE, true)
        }


//        binding.lv.apply {
//         //   addView(buildNestedLayout(this@MainActivity, 1, 10, textView))
//            addView(textView)
//        }


    }
}