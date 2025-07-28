package com.example.hook

import android.os.Parcel
import android.os.Parcelable

data class Config(
    var deOptimizedMode: Int = 0,
): Parcelable {

    inner class Builder {

        private var mode = 0

        fun deOptimizedMod(mode: Int): Builder {
            this.mode = mode
            return this
        }

        fun build(): Config {
            return Config(
                mode
            )
        }

    }

    constructor(parcel: Parcel) : this(parcel.readInt()) {
    }

    companion object CREATOR : Parcelable.Creator<Config> {
        override fun createFromParcel(parcel: Parcel): Config {
            return Config(parcel)
        }

        override fun newArray(size: Int): Array<Config?> {
            return arrayOfNulls(size)
        }
    }

    override fun describeContents(): Int = 0

    override fun writeToParcel(dest: Parcel, flags: Int) {
        dest.writeInt(deOptimizedMode)
    }

}