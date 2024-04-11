package com.lolicononline.smartsystemapplication.ui.config

import androidx.lifecycle.LiveData
import androidx.lifecycle.MutableLiveData
import androidx.lifecycle.ViewModel

class ConfigViewModel : ViewModel() {

    private val _text = MutableLiveData<String>().apply {
        value = "This is config Fragment"
    }
    val text: LiveData<String> = _text
}