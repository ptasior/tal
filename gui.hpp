template<class T>
void Widget::addWidget(T* w)
{
	addForeignWidget(w); // To enable virtual methods
}

template<class T>
void Widget::removeWidget(T* w)
{
	removeForeignWidget(w); // To enable virtual methods
}

