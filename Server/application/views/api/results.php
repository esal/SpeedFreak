<?php echo "<?"; ?>xml version="1.0" encoding="utf-8" <?php echo "?>"; ?>

<results>
    <?php $i=1; foreach ($results as $r){ ?>   <result username="<?php echo $r->username; ?>" position="<?php echo $i; ?>" date="<?php echo $r->result_date; ?>" <?php if (isset($show_unit) AND $show_unit==false) echo 'unit="'.$r->unit.'" '; ?>value="<?php echo $r->value; ?>" />
<?php $i++; } ?></results>
