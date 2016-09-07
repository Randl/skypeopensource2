#Versions
* multysend -- попытка послать несколько скайп-сообщений используя новый 5.5 протокол.
* multysend38 -- попытка послать несколько скайп-сообщений используя старый 3.8 протокол.
* goodsend -- попытка сделать нормальный send сообщений на базе multysend6l
но по новому задампленному протоколу sendproto1 от 14.01.2016г.
Это идеальная сессия. Сделаная один-в-один
* goodsend2 -- нормальный send как в goodsend но уже с моими модификациями.
с моим chat_string_id и временем.
* goodsend3 -- нормальный send как в goodsend2 но уже с моими модификациями
для посылки в уже существующую сессию.
* goodsend4 -- добавление своего перехвата printf.
* goodsendrelay -- форк goodsend4 для добавления возможности работы через relay.
проект работает
* goodsendrelay2 -- для фиксов и изменений goodsendrelay.
* goodsendrelay3 -- для изменения протокола для повторных send-ов
чтобы чередующаяся посылка-отправка работала
обработка cmd10 с флагом
* good75sendrelay -- for test send via relays on skype7.5 version

##Files

* chatsend_newchat_init.c -- initial send with create new session packet sequences (extended crypto for signing).
* chatsend_restorechat.c -- send msg to already created session with confirm header_id after send on other side
* chatsend_restorechat2.c -- send msg to already created session, simply fork on prev, as end without confirmation packets. Restore chat session pkts (alternative send final packets).