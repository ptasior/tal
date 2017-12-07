template<class T>
void Widget::addWidget(T* w)
{
	setupChild(w);
	mForeignWidgets.push_back(w);
}

