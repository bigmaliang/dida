LOCALBASE = ../../
include $(LOCALBASE)Make.env
include $(BASEDIR)Make.env

FILES = b/base.css \
        b/index.css \
        b/readme.css \
        b/admin/spd/post.css b/admin/spd/robot.css \
        b/member/regist.css b/member/account.css b/member/reset.css b/member/home.css \
        b/bore/bore.css b/bore/op.css \
        b/plan/info.css b/plan/mine.css \
        b/inbox/system.css b/inbox/private.css b/inbox/sent.css \
        b/system/view.css b/system/comment.css b/system/plan.css

all: preload $(FILES)

preload:
	@mkdir -p b/
	@mkdir -p b/admin/spd/
	@mkdir -p b/member/
	@mkdir -p b/bore/
	@mkdir -p b/plan/
	@mkdir -p b/inbox/
	@mkdir -p b/system/

b/base.css: reset.css base.css layout.css modules/overlay.css

b/index.css: modules/indicator.css modules/dateinput/dateinput.css \
             modules/tooltip.css \
             modules/noty/jquery.noty.css modules/noty/noty_theme_mitgux.css \
             index/index.css
b/readme.css: readme.css

b/admin/spd/post.css: modules/indicator.css admin/spd/post.css
b/admin/spd/robot.css: admin/spd/robot.css

b/member/regist.css: modules/noty/jquery.noty.css modules/noty/noty_theme_mitgux.css \
                     modules/indicator.css
b/member/account.css: modules/box.css modules/minibtn.css modules/indicator.css \
                      modules/dateinput/dateinput.css \
                      member/account.css
b/member/reset.css: modules/indicator.css
b/member/home.css: modules/tooltip.css modules/miniicon.css modules/pagenav.css \
                   member/home.css

b/bore/bore.css: modules/indicator.css bore/bore.css
b/bore/op.css: modules/indicator.css bore/mku_skin.css bore/mku_set.css bore/op.css

b/plan/info.css: plan/info.css
b/plan/mine.css: modules/dateinput/dateinput.css modules/tooltip.css \
                 modules/indicator.css modules/minibtn.css modules/pagenav.css \
                 modules/noty/jquery.noty.css modules/noty/noty_theme_mitgux.css \
                 plan/mine.css

b/inbox/system.css: modules/accordion.css modules/box.css modules/pagenav.css \
                    inbox/inbox.css
b/inbox/private.css: modules/accordion.css modules/box.css modules/pagenav.css \
                     inbox/inbox.css
b/inbox/sent.css: modules/accordion.css modules/box.css modules/pagenav.css \
                  inbox/inbox.css

b/system/view.css: modules/accordion.css modules/box.css \
                   modules/dateinput/dateinput.css modules/tablesorter/tablesorter.css \
                   system/system.css
b/system/comment.css: modules/accordion.css modules/box.css \
                   modules/dateinput/dateinput.css \
                   system/system.css system/comment.css
b/system/plan.css: modules/accordion.css modules/box.css \
                   modules/dateinput/dateinput.css modules/pagenav.css \
                   system/system.css

%.css:
	$(CSSGLUE) $^ > $@


clean:
	@rm -f $(FILES)
