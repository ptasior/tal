#include <algorithm>

template<class T>
void Widget::addWidget(T* w)
{
	mForeignWidgets.push_back(w);
	setupChildren();
}

template<class T>
void Widget::removeWidget(T* w)
{
	auto f = std::remove_if(
				mForeignWidgets.begin(),
				mForeignWidgets.end(),
				[&w](Widget *i){return i == w;}
			);
	if(f == mForeignWidgets.end())
		Log() << "Widget: Cannot find widget to remove";
	else
		mForeignWidgets.erase(f, mForeignWidgets.end());
}

