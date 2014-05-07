<?cs def:plan_sdate(repeat, sdate) ?>
<?cs if:repeat == #0 ?>
<?cs var:sdate ?>
<?cs elif:repeat == #1 ?>
每日
<?cs elif:repeat == #2 ?>
每周 <?cs var:sdate ?>
<?cs /if ?>
<?cs /def ?>

<?cs def:plan_dad(dad) ?><?cs if:dad==0 ?>找车<?cs elif:dad==1 ?>邀人<?cs /if ?><?cs /def ?>

<?cs def:plan_subscribe(val, which) ?><?cs if:bitop.and(val, which) == #which ?>checked="checked"<?cs /if ?><?cs /def ?>

<?cs def:plan_disabled(a, b) ?><?cs if:a==b ?>disabled="disabled"<?cs /if ?><?cs /def ?>
<?cs def:plan_hide(a, b) ?><?cs if:a==b ?> hide<?cs /if ?><?cs /def ?>
