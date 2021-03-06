[English](https://github.com/unclesal/xenon/blob/master/README.md)

# XENON

Мульти-агентное моделирование воздушного трафика и работы аэропортов.

# Компиляция и установка

Не надо его пока что ни компилировать, ни ставить. Все равно ничего осмысленного он еще не делает.

# Термины и соглашения

- Все исходные коды являются открытыми, кроме серверной части. Это потому, что аренда виртуальных серверов - не бесплатное занятие и я хотел бы избежать бесконтрольного роста агентов.

# Я использую

1. Прекраснейший (и официально приобретенный) симулятор полета [X-Plane](https://www.x-plane.com/)

2. Свободно-распространяемый X-Plane сценарий аэропорта Кольцово (Екатеринбург, Россия). Оригинальная версия может
быть получена [отсюда](http://x-flight.su/ural.php). Я его малость модифицировал под свои нужды, а именно: убрал
из оригинала все статические самолеты и наземный трафик. Модицицированная версия - [здесь](https://drive.google.com/file/d/1ZsokPeAb87V5MtS3KQzZL7dX1c05e4Z5/view?usp=sharing).

3. Многопользовательскую библиотеку нового поколения X-Plane Multiplay Library, поддерживающую стандарты OpenGL, Vulcan, Metal, 
поддерживающую систему TCAS и трехмерные модели CSL OBJ8, написанную [TwinFan](https://github.com/TwinFan/XPMP2)
Вам не нужно ее отдельно устанавливать, поскольку она компилируется статически. Но обратите особое внимание
на дополнительные файлы в подкаталоге [Resources](https://github.com/TwinFan/XPMP2/tree/master/Resources) - они понадобятся.

4. Саму библиотеку 3D моделей формата OBJ8 CSL, собранную [Oktalist](https://forums.x-plane.org/index.php?/files/file/37041-bluebell-obj8-csl-packages/). 
Вы также можете скачать ее с [Гугл Диска](https://drive.google.com/drive/folders/0B8hGoqCV5Z5AcTM4ZWhIX2RZT2M), если
такой способ более удобен для Вас. Важно, чтобы в каталоге **X-Plane ROOT/Resources/plugins/xenon** был подкаталог
**Resources/CSL** с указанными моделями.

# Видяшки

1. [Улет в пампасы](https://www.youtube.com/watch?v=wfAPAnU-FBk)
2. [Посадка](https://www.youtube.com/watch?v=SvydTdDbwhs)
3. [Коробочка (полет по кругу)](https://youtu.be/CE1rVc9RiTU?list=PLnt2A94rJYDaAttvRDuDdlWqQyYRg16bf)
