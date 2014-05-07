LOCALBASE = ../../
include $(LOCALBASE)Make.env
include $(BASEDIR)Make.env

FILES = b/base.js \
        b/index.js \
        b/admin/pos.js \
        b/admin/spd/58.user.js b/admin/spd/58.js \
        b/admin/spd/ganji.user.js b/admin/spd/ganji.js \
        b/admin/spd/edeng.user.js b/admin/spd/edeng.js \
        b/admin/spd/liebiao.user.js b/admin/spd/liebiao.js \
        b/admin/spd/post.js b/admin/spd/robot.js \
        b/member/regist.js b/member/reset.js b/member/account.js b/member/home.js \
        b/bore/bore.js b/bore/op.js \
        b/plan/info.js b/plan/mine.js \
        b/inbox/system.js \
        b/system/ie.js b/system/view.js b/system/comment.js b/system/plan.js

all: preload $(FILES)

preload:
	@mkdir -p b/
	@mkdir -p b/admin/spd/
	@mkdir -p b/member/
	@mkdir -p b/bore/
	@mkdir -p b/plan/
	@mkdir -p b/inbox/
	@mkdir -p b/system/

b/base.js: pub/jquery-1.7.1.js pub/jquery.cookie.js pub/jquery.md5.js \
           pub/jquery.bmoon.js pub/jquery.hotkeys.js pub/jquery.mninpval.js \
           pub/jquerytools/src/toolbox/toolbox.expose.js \
           pub/jquerytools/src/overlay/overlay.js \
           pub/bmoon.js bmoon.dida.js

b/index.js: pub/jquerytools/src/dateinput/dateinput.js \
            pub/jquerytools/src/tooltip/tooltip.js \
            pub/noty/js/jquery.noty.js \
            pub/jquery.mnblink.js index.js

b/admin/spd/58.user.js: admin/spd/58.user.js
b/admin/spd/58.js: pub/jquery-1.7.1.js pub/jquery.cookie.js \
                   pub/bmoon.js bmoon.dida.js admin/spd/58.js

b/admin/spd/ganji.user.js: admin/spd/ganji.user.js
b/admin/spd/ganji.js: pub/jquery-1.7.1.js pub/jquery.cookie.js \
                      pub/bmoon.js bmoon.dida.js admin/spd/ganji.js

b/admin/spd/edeng.user.js: admin/spd/edeng.user.js
b/admin/spd/edeng.js: pub/jquery-1.7.1.js pub/jquery.cookie.js \
                      pub/bmoon.js bmoon.dida.js admin/spd/edeng.js

b/admin/spd/liebiao.user.js: admin/spd/liebiao.user.js
b/admin/spd/liebiao.js: pub/jquery-1.7.1.js pub/jquery.cookie.js \
                        pub/bmoon.js bmoon.dida.js admin/spd/liebiao.js

b/admin/pos.js: admin/pos.js
b/admin/spd/post.js: admin/spd/post.js
b/admin/spd/robot.js: admin/spd/robot.js

b/member/regist.js: pub/noty/js/jquery.noty.js member/regist.js
b/member/reset.js: member/reset.js
b/member/account.js: pub/jquerytools/src/dateinput/dateinput.js member/account.js
b/member/home.js: pub/jquerytools/src/tooltip/tooltip.js pub/jquery.mnnav.js \
                  member/home.js

b/bore/bore.js: bore/bore.js
b/bore/op.js: pub/markitup/markitup/jquery.markitup.js bore/mku_set.js \
              pub/jqueryUploader/src/jquery.uploader.js bore/op.js

b/plan/info.js: plan/info.js
b/plan/mine.js: pub/jquerytools/src/dateinput/dateinput.js \
                pub/noty/js/jquery.noty.js \
                pub/jquery.mnnav.js pub/jquerytools/src/tooltip/tooltip.js \
                pub/jquery.mnblink.js \
                plan/mine.js

b/inbox/system.js: pub/jquery.mnnav.js inbox/system.js

b/system/ie.js: pub/flot/excanvas.min.js
b/system/view.js: pub/jquerytools/src/dateinput/dateinput.js \
                  pub/jquery.mntable.js pub/jquery.tablesorter.js \
                  pub/flot/jquery.flot.js pub/flot/jquery.flot.time.js system/view.js
b/system/comment.js: pub/jquerytools/src/dateinput/dateinput.js \
                  system/comment.js
b/system/plan.js: pub/jquerytools/src/dateinput/dateinput.js \
                  pub/jquery.mnnav.js \
                  system/plan.js

%.js:
	@echo -n > $@
	$(JSGLUE) $^ > $@


clean:
	@rm -f $(FILES)
