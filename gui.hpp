template<class T>
void Widget::addWidget(T* w)
{
	mForeignWidgets.push_back(w);
	setupChildren();
}

