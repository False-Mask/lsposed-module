package com.example.test

import android.content.Context
import android.view.View
import android.widget.FrameLayout
import android.widget.LinearLayout
import android.widget.TextView

fun buildLayout(view: View,dep: Int): View {
    if(dep == 0) {
        return view
    }
    val layout = buildLayout(view, dep - 1)
    val fl = FrameLayout(view.context).apply {
        addView(layout)
    }
    return fl
}


fun buildNestedLayout(ctx: Context, dep: Int, n: Int,lastView:View): View {
    if(dep > n) {
        return lastView
    }
    val layout = LinearLayout(ctx).also { ll->
        ll.orientation = LinearLayout.HORIZONTAL
        for(i in 0 until dep) {
            ll.addView(TextView(ctx).also { tv->
                tv.text = i.toString()
            })
        }
        ll.addView(buildNestedLayout(ctx, dep + 1, n,lastView))
    }

    return layout

}